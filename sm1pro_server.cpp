//
// Created by fiorentinoing on 26/07/17.
//

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "sm1pro.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using sm1pro::SaveRequest;
using sm1pro::SaveReply;
using sm1pro::SmartMeter1;

// Logic and data behind the server's behavior.
class SmartMeter1ServiceImpl final : public SmartMeter1::Service {
    Status SayHello(ServerContext* context, const SaveRequest* request,
                    SaveReply* reply) override {
        std::string messageToSave =
                request->sensor_type() + " "
                + request->sensor_name() + " "
                + request->data_type() + " "
                + request->raw_value()+ " "
                + request->timestamp();
        reply->set_message(messageToSave);
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    SmartMeter1ServiceImpl service;

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

int main(int argc, char** argv) {
    RunServer();

    return 0;
}
