
#include <iostream>
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


using data_transfer::Data_msg_req;
using data_transfer::Data_msg_res;
using data_transfer::Data_Send;
using std::chrono::system_clock;

using namespace std;




class Data_SendImpl final : public Data_Send::Service
{

Status send_data(ServerContext* context, const Data_msg_req* req, Data_msg_res* res) override
{
	std::string req_str = req-> string_value();
	int req_num = req-> int_value();
 	std::cout <<"num: "<< req_num << std::endl << "string: "<< req_str << std::endl;

 	res->set_status_name(req_str + " Succeded!");
 	res->set_status(0);

 	return Status::OK;
}

};




void run_server()
{
  std::string server_address("0.0.0.0:50051");
  Data_SendImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}


int main()
{
	run_server();
}