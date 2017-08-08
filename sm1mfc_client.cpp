//
// Created by fiorentinoing on 26/07/17.
//

#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <ctime>

#include <grpc++/grpc++.h>

#include "sm1pro.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using sm1pro::SaveRequest;
using sm1pro::SaveReply;
using sm1pro::SmartMeter1;

class SmartMeter1Client {
public:
    SmartMeter1Client(std::shared_ptr<Channel> channel)
            : stub_(SmartMeter1::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string& sensor_name) {
        
        // Data we are sending to the server.
        SaveRequest request;
        request.set_sensor_name(sensor_name);
        request.set_sensor_type("MFC");
        request.set_data_type("current_mass_flow_rate");
        request.set_raw_value("0.15");
        request.set_timestamp("2017-07-31T21:30:00.000Z");

        // Container for the data we expect from the server.
        SaveReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->SayHello(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cerr << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }

    }

private:
    std::unique_ptr<SmartMeter1::Stub> stub_;
};

int main(int argc, char** argv) {

    std::vector<std::string> sensorList = {"01", "02"};

    while (true)
    {
        try{
            for (auto it=sensorList.cbegin(), end=sensorList.cend(); it!=end; ++it)
            {
                SmartMeter1Client SmartMeter1Client(grpc::CreateChannel(
                        "localhost:50051", grpc::InsecureChannelCredentials()));
                std::string sensor_name(*it);
                std::string reply = SmartMeter1Client.SayHello(sensor_name);
                std::cout << "SmartMeter1 received: " << reply << std::endl;
            }
            sleep(1);
        }
        catch(...)
        {
            std::cerr << "Unknown catched error.." << std::endl;
        }
    }
    
    return 0;
}
