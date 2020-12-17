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
#pragma once

// generated
#include <protos/chat/chat_room.grpc.pb.h>

// external
#include <grpc++/server.h>

namespace ltb::example {

class ExampleService : public ChatRoom::Service {
public:
    ~ExampleService() override;

    grpc::Status DispatchAction(grpc::ServerContext* context, Action const* request, util::Result* response) override;

    grpc::Status
    PokeUser(grpc::ServerContext* context, grpc::ServerReader<User::Id>* reader, util::Result* response) override;

    grpc::Status SearchMessages(grpc::ServerContext*                 context,
                                google::protobuf::StringValue const* request,
                                grpc::ServerWriter<UserMessage>*     writer) override;

    grpc::Status GetMessages(grpc::ServerContext*                                         context,
                             grpc::ServerReaderWriter<ChatMessageResult, ChatMessage_Id>* stream) override;

    grpc::Status
    GetUpdates(grpc::ServerContext* context, User::Id const* request, grpc::ServerWriter<Update>* writer) override;
};

class ExampleServer {
public:
    explicit ExampleServer(std::string const& host_address = "");

    auto grpc_server() -> grpc::Server&;
    auto shutdown() -> void;

private:
    ExampleService                service_;
    std::unique_ptr<grpc::Server> server_;
};

} // namespace ltb::example
