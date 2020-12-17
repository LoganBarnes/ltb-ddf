// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Networking
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#include "example_server.hpp"

#include <grpc++/server_builder.h>

namespace ltb::example {

ExampleService::~ExampleService() {}

grpc::Status
ExampleService::DispatchAction(grpc::ServerContext* /*context*/, Action const* request, util::Result* response) {
    std::cout << "S: Action: " << request->ShortDebugString() << std::endl;
    response->mutable_success();
    return grpc::Status::OK;
}

grpc::Status
ExampleService::PokeUser(grpc::ServerContext* context, grpc::ServerReader<User::Id>* reader, util::Result* response) {
    return Service::PokeUser(context, reader, response);
}

grpc::Status ExampleService::SearchMessages(grpc::ServerContext*                 context,
                                            google::protobuf::StringValue const* request,
                                            grpc::ServerWriter<UserMessage>*     writer) {
    return Service::SearchMessages(context, request, writer);
}

grpc::Status ExampleService::GetMessages(grpc::ServerContext*                                         context,
                                         grpc::ServerReaderWriter<ChatMessageResult, ChatMessage_Id>* stream) {
    return Service::GetMessages(context, stream);
}

grpc::Status
ExampleService::GetUpdates(grpc::ServerContext* context, User::Id const* request, grpc::ServerWriter<Update>* writer) {
    return Service::GetUpdates(context, request, writer);
}

ExampleServer::ExampleServer(std::string const& host_address) {
    grpc::ServerBuilder builder;
    if (!host_address.empty()) {
        // std::cout << "S: " << host_address << std::endl;
        builder.AddListeningPort(host_address, grpc::InsecureServerCredentials());
    }
    builder.RegisterService(&service_);
    server_ = builder.BuildAndStart();
}

auto ExampleServer::grpc_server() -> grpc::Server& {
    return *server_;
}

auto ExampleServer::shutdown() -> void {
    server_->Shutdown();
}

} // namespace ltb::example
