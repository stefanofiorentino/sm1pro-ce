//
// Created by fiorentinoing on 26/07/17.
//
/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#include <grpc++/grpc++.h>

#include "sm1pro.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using sm1pro::SaveRequest;
using sm1pro::SaveReply;
using sm1pro::Greeter;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
            : stub_(Greeter::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string& sensor_name) {
        
        // Data we are sending to the server.
        SaveRequest request;
        request.set_sensor_name(sensor_name);
        request.set_sensor_type("MFC");
        request.set_data_type("current_mass_flow_rate");
        request.set_raw_value("0.15");
        request.set_timestamp("yyyy-mm-ddThh:mm:ss.mmmZ");

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
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }

    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {

    std::vector<std::string> sensorList = {"01", "02"};

    while (true)
    {
        try{
            for (auto it=sensorList.cbegin(), end=sensorList.cend(); it!=end; ++it)
            {
                GreeterClient greeter(grpc::CreateChannel(
                        "localhost:50051", grpc::InsecureChannelCredentials()));
                std::string sensor_name(*it);
                std::string reply = greeter.SayHello(sensor_name);
                std::cout << "Greeter received: " << reply << std::endl;
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
