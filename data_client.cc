#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector> 

#include <grpcpp/grpcpp.h>

#include "data_specification.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

using data_transfer::Data_msg_req;
using data_transfer::Data_msg_res;
using data_transfer::Data_Send;

using data_transfer::BigFileChunk;
using data_transfer::BigFileAck;

using std::ifstream;

class Data_Transfer_Client
{
public:
	Data_Transfer_Client(std::shared_ptr<Channel> channel) : stub_(Data_Send::NewStub(channel))	{}


	std::string send_data(const int& int_val, const std::string& str_val)
	{
		Data_msg_req request;

		request.set_string_value(str_val);
		request.set_int_value(int_val);


		Data_msg_res response;

		ClientContext context;


		Status status = stub_->send_data(&context, request, &response);


		if (status.ok()) 
		{
			return response.status_name();
		} else {
			std::cerr<< status.error_code() << ": " << status.error_message() << std::endl;
			return "RPC Failed!";
		}
	}

	std::string UploadFile(const std::string filename)
	{
		const int Max_Chunk_size = 1024;//1826626; //1KB
		ClientContext context;
		
		std::vector<char> buffer (Max_Chunk_size + 1, 0);

		
		BigFileAck ack; //Response container
		int read_bytes = 0;
		int sent_chunks = 0;

		std::cout<<"opening file: "<< filename<<" ..."<<std::endl;
		ifstream in_file(filename,std::ifstream::binary);

		if(!in_file)
		{
			std::cerr<<"File: "<<filename<<" Does Not Exist!"<<std::endl;
			return "File Transfer Failed!";

		}
		std::cout<<"opening Succeded! "<<std::endl;
		in_file.seekg (0, in_file.end);
		int file_length = in_file.tellg();
		in_file.seekg (0, in_file.beg);

		std::unique_ptr< ClientWriter<BigFileChunk> > writer(
        	stub_->UploadFile(&context, &ack));
			
		int bytes_to_read = std::min(Max_Chunk_size,(file_length-read_bytes));
		
		while(writer&&in_file.read(buffer.data(),bytes_to_read)&&bytes_to_read)
		{
			
			BigFileChunk chunk;
			

			//in_file.read(buffer.data(),bytes_to_read);
			read_bytes += bytes_to_read;
			
			chunk.set_data(buffer.data());
			chunk.set_filename(filename);
			chunk.set_chunknumber(sent_chunks++);
			chunk.set_sizeinbytes(file_length);
			chunk.set_payloadsize(bytes_to_read);
			
			if (read_bytes == file_length) chunk.set_islastchunk(true);
			else chunk.set_islastchunk(false);
			
			if (!writer->Write(chunk)) 
			{
				// Broken stream.
				std::cerr<<"Broken Stream!"<<std::endl;
				return "File Transfer Failed!";

				break;
      		}
			std::cout<<std::endl<<"<--------------------------------------------------------------->"<<std::endl;
			std::cout<<"Chunk #"<<sent_chunks <<" Size: "<< bytes_to_read <<std::endl;
			std::cout<<chunk.data().c_str()<<std::endl;
			std::cout<<"Sent "<<read_bytes<<"/"<<file_length <<" Bytes!"<<std::endl<<std::endl;

			bytes_to_read = std::min(Max_Chunk_size,(file_length-read_bytes));
			system("read -p 'Press Enter to continue...' var");
		}
		in_file.close();
		writer->WritesDone();
    	Status status = writer->Finish();
    	std::cout<<"The server received "<<ack.sizeinbytes() <<" Bytes!"<<std::endl;	
		std::cout<<"Trasfer Status: "<<ack.err()<<std::endl;
	




		return "File Trasfer OK!";
	}

private:
	std::unique_ptr<Data_Send::Stub> stub_;
};


int main(int argc, char const *argv[])
{
	
	// Check the number of parameters
    if (argc < 2|| ((argc-1)%2)==1) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0];
        std::cerr << "options:"<< std::endl<<
        			"\t  -str <str_msg>"<< std::endl<<
        			"\t  -num <int_msg>"<<std::endl<<
        			"\t  -file <filename>"<<std::endl;;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }


    std::string str_value;
    std::string filename="";
    int num_value;

    //std::cout<<argc<<std::endl;
    for (int i=1;i<argc;i+=2){
    //	std::cout<<i<<","<<i+1<<std::endl;
    //	std::cout<<argv[i]<<","<<argv[i+1]<<std::endl;
    	std::string arg = argv[i];

    	if (arg=="-num")
    	{
    		std::string::size_type sz;
    		num_value = std::stoi(argv[i+1],&sz);
    	}else if(arg=="-str")
    	{
    		str_value = argv[i+1];
    	} else if (arg=="-file") 
    	{
    		filename = argv[i+1];
    	}
    	else std::cerr<<"wrong argument!"<<std::endl;
    	
    }

    //std::cout<<str_value<<","<<num_value<<std::endl;
	Data_Transfer_Client client(grpc::CreateChannel(
		"0.0.0.0:50051",grpc::InsecureChannelCredentials()));

	std::string result,transfer_result;

	result = client.send_data(num_value,str_value);
	transfer_result = client.UploadFile(filename);
	//std::cout << result<<std::endl;
	std::cout << transfer_result<<std::endl;

	return 0;
}