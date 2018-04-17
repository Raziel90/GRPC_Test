#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "data_specification.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriterInterface;

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
			std::cout<< status.error_code() << ": " << status.error_message() << std::endl;
			return "RPC Failed!";
		}
	}
	BigFileAck UploadFile(const std::string filename)
	{

		BigFileChunk chunk;


		//Response container
		BigFileAck ack;

		return ack;
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
        			"\t  -num <int_msg>"<<std::endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }


    std::string str_value;
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
    	} else std::cerr<<"wrong argument!"<<std::endl;
    	
    }

    std::cout<<str_value<<","<<num_value<<std::endl;
	Data_Transfer_Client client(grpc::CreateChannel(
		"0.0.0.0:50051",grpc::InsecureChannelCredentials()));

	std::string result;
	result = client.send_data(num_value,str_value);
	std::cout << result<<std::endl;
	return 0;
}