// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
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

// project
#include "example_service.hpp"
#include "ltb/net/server/rpc_function_types.hpp"

// external
#include <grpc++/server_builder.h>

namespace ltb::example {
namespace {

auto set_callbacks(ltb::net::AsyncServer<ChatRoom::AsyncService>* async_server, ExampleService* service) -> void {
    async_server->register_rpc(&ChatRoom::AsyncService::RequestDispatchAction,
                               [service](Action const& action, net::AsyncServerUnaryWriter<util::Result> writer) {
                                   std::cout << "S: " << action.ShortDebugString() << std::endl;
                                   util::Result response;
                                   auto         status = service->handle_action(action, &response);
                                   writer.finish(response, status);
                               });

    async_server->register_rpc(&ChatRoom::AsyncService::RequestPokeUser/*,
                               [service](Action const& action, net::AsyncServerUnaryWriter<util::Result> writer) {
                                   std::cout << "S: " << action.ShortDebugString() << std::endl;
                                   util::Result response;
                                   auto         status = service->handle_action(action, &response);
                                   writer.finish(response, status);
                               }*/);

    async_server->register_rpc(&ChatRoom::AsyncService::RequestSearchMessages);
    async_server->register_rpc(&ChatRoom::AsyncService::RequestGetMessages);
    async_server->register_rpc(&ChatRoom::AsyncService::RequestGetUpdates);

#if 0
    ChatRoom::AsyncService async_service;

    // Unary
    {
        grpc::ServerContext*                           context = nullptr;
        Action*                                        request = nullptr;
        grpc::ServerAsyncResponseWriter<util::Result>* writer  = nullptr;
        grpc::ServerCompletionQueue*                   cq      = nullptr;
        void*                                          tag     = nullptr;
        (async_service.*(&ChatRoom::AsyncService::RequestDispatchAction))(context, request, writer, cq, cq, tag);

        util::Result response;
        writer->Finish(response, grpc::Status::OK, tag);
    }

    // Client stream
    {
        grpc::ServerContext*                             context = nullptr;
        grpc::ServerAsyncReader<util::Result, User::Id>* reader  = nullptr;
        grpc::ServerCompletionQueue*                     cq      = nullptr;
        void*                                            tag     = nullptr;
        (async_service.*(&ChatRoom::AsyncService::RequestPokeUser))(context, reader, cq, cq, tag);

        User::Id     request;
        util::Result response;
        reader->Read(&request, tag);
        reader->Finish(response, grpc::Status::OK, tag);
    }

    // Server stream
    {
        grpc::ServerContext*                  context = nullptr;
        google::protobuf::StringValue*        request = nullptr;
        grpc::ServerAsyncWriter<UserMessage>* writer  = nullptr;
        grpc::ServerCompletionQueue*          cq      = nullptr;
        void*                                 tag     = nullptr;
        (async_service.*(&ChatRoom::AsyncService::RequestSearchMessages))(context, request, writer, cq, cq, tag);

        UserMessage response;
        writer->Write(response, tag);
        writer->Finish(grpc::Status::OK, tag);
    }

    // Bi-directional stream
    {
        grpc::ServerContext*                                               context = nullptr;
        grpc::ServerAsyncReaderWriter<ChatMessageResult, ChatMessage::Id>* stream  = nullptr;
        grpc::ServerCompletionQueue*                                       cq      = nullptr;
        void*                                                              tag     = nullptr;
        (async_service.*(&ChatRoom::AsyncService::RequestGetMessages))(context, stream, cq, cq, tag);

        ChatMessage::Id   request;
        ChatMessageResult response;
        stream->Read(&request, tag);
        stream->Write(response, tag);
        stream->Finish(grpc::Status::OK, tag);
    }

    // Server stream
    {
        grpc::ServerContext*             context = nullptr;
        User::Id*                        request = nullptr;
        grpc::ServerAsyncWriter<Update>* writer  = nullptr;
        grpc::ServerCompletionQueue*     cq      = nullptr;
        void*                            tag     = nullptr;
        (async_service.*(&ChatRoom::AsyncService::RequestGetUpdates))(context, request, writer, cq, cq, tag);

        Update response;
        writer->Write(response, tag);
        writer->Finish(grpc::Status::OK, tag);
    }
#endif
}

} // namespace

ExampleServer::ExampleServer(std::string const& host_address) : async_server_(host_address) {
    run_thread_ = std::thread([this] {
        ExampleService service;
        set_callbacks(&async_server_, &service);
        async_server_.run();
    });
}

ExampleServer::~ExampleServer() {
    shutdown();
    run_thread_.join();
}

auto ExampleServer::grpc_server() -> grpc::Server& {
    return async_server_.grpc_server();
}

auto ExampleServer::shutdown() -> void {
    async_server_.shutdown();
}

} // namespace ltb::example
