
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
//#include "helper.h"
#include "data_specification.grpc.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::ServerReader;


using data_transfer::Data_msg_req;
using data_transfer::Data_msg_res;
using data_transfer::Data_Send;
using data_transfer::BigFileChunk;
using data_transfer::BigFileAck;


using std::ofstream;
using std::chrono::system_clock;

using namespace std;




class Data_SendImpl final : public Data_Send::Service
{

Status send_data(ServerContext* context, const Data_msg_req* req, Data_msg_res* res) override
{


	std::string req_str = req-> string_value();
	int req_num = req-> int_value();
 	std::cout <<"SERVER: num: "<< req_num <<  "   string: "<< req_str << std::endl;

 	res->set_status_name(req_str + " Succeded!");
 	res->set_status(0);

 	return Status::OK;
}

Status UploadFile(ServerContext* context, ServerReaderWriter< BigFileAck, BigFileChunk>* reader)override 
{
	//std::cout<<"<.........................SERVER....................................>"<<std::endl;
	const int Max_Chunk_size = 1024;//1826626; //1KB

	ofstream out_file;
	BigFileChunk chunk_placeholder;
	system_clock::time_point start_time = system_clock::now();
	
	int chunk_counter = 0;
	int loaded_bytes = 0;

	std::cout<<"SERVER: File Transfer Requested!"<<std::endl;

	while(reader->Read(&chunk_placeholder))
	{
		
		
		if (chunk_counter == 0)
		{
			//open File
			//std::cout<<"<...................................................................>"<<std::endl;
			//std::cout<<"Filename: "<< chunk_placeholder.filename() <<std::endl;
			//std::cout<<"Size: "<< chunk_placeholder.sizeinbytes() <<std::endl;

			//std::cout<<"<...................................................................>"<<std::endl<<std::endl<<std::endl;
			out_file.open(chunk_placeholder.filename(),std::ofstream::binary|std::ofstream::trunc);
		}
		
		int bytes_to_load=chunk_placeholder.payloadsize();
		std:string payload = chunk_placeholder.data();
		
		loaded_bytes=loaded_bytes+bytes_to_load;

		//std::cout<<std::endl<<"<--------------------------------------------------------------->"<<std::endl;
		//std::cout<<"Chunk Received #"<<chunk_placeholder.chunknumber()<<" Payload Size: "<<bytes_to_load<<std::endl;
		//std::cout<<"LAST Chunk Received #"<<chunk_counter<<std::endl;
		//std::cout<<"Received "<<loaded_bytes<<"/"<<chunk_placeholder.sizeinbytes() <<" Bytes!"<<std::endl;
		chunk_counter++;
		

		// Write Data!
		out_file.write(payload.c_str(),bytes_to_load);
		

		BigFileAck response;
		response.set_filename(chunk_placeholder.filename());
		response.set_sizeinbytes(loaded_bytes);
		response.set_chunknumber(chunk_counter);
		response.set_status("OK!");
		if (chunk_counter < chunk_placeholder.chunknumber()) 
		{
			response.set_status("File Transfer Failed!");
			reader->Write(response);
			return Status::CANCELLED;
		}
		out_file.flush();
		reader->Write(response);
	}
	
	out_file.close();
	std::cout<<"SERVER: File Transfer Completed! "<<std::endl;
	

	return Status::OK;
}

};




void run_server(std::string server_address)
{
  
  Data_SendImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "SERVER: listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}


int main()
{
	run_server("0.0.0.0:50051");
}