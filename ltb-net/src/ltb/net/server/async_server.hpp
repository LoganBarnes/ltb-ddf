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

// project
#include "async_server_rpc.hpp"
#include "async_unary_call_data.hpp"
#include "ltb/net/tagger.hpp"

// external
#include <grpc++/server.h>
#include <grpc++/server_builder.h>

// standard
#include <functional>
#include <unordered_map>

namespace ltb::net {

template <typename Service>
class AsyncServer {
public:
    explicit AsyncServer(std::string const& host_address);

    auto grpc_server() -> grpc::Server&;

    /// \brief Blocks the current thread.
    auto run() -> void;

    auto shutdown() -> void;

    template <typename BaseService, typename Request, typename Response>
    auto register_rpc(UnaryAsyncRpc<BaseService, Request, Response>             unary_call_ptr,
                      typename ServerCallbacks<Request, Response>::UnaryConnect on_connect,
                      DisconnectCallback                                        on_disconnect = nullptr) -> void;

    template <typename BaseService, typename Request, typename Response>
    auto register_rpc(ClientStreamAsyncRpc<BaseService, Request, Response> call_ptr) -> void;

    template <typename BaseService, typename Request, typename Response>
    auto register_rpc(ServerStreamAsyncRpc<BaseService, Request, Response> call_ptr) -> void;

    template <typename BaseService, typename Request, typename Response>
    auto register_rpc(BidirectionalStreamAsyncRpc<BaseService, Request, Response> call_ptr) -> void;

private:
    std::mutex                                   mutex_;
    Service                                      service_;
    std::unique_ptr<grpc::ServerCompletionQueue> completion_queue_;
    std::unique_ptr<grpc::Server>                server_;

    ServerTagger tagger_;

    std::unordered_map<void*, std::unique_ptr<detail::AsyncServerRpc<Service>>> rpc_call_data_;
};

template <typename Service>
AsyncServer<Service>::AsyncServer(std::string const& host_address) {
    std::lock_guard lock(mutex_);

    grpc::ServerBuilder builder;
    if (!host_address.empty()) {
        builder.AddListeningPort(host_address, grpc::InsecureServerCredentials());
    }
    builder.RegisterService(&service_);
    completion_queue_ = builder.AddCompletionQueue();
    server_           = builder.BuildAndStart();
}

template <typename Service>
auto AsyncServer<Service>::grpc_server() -> grpc::Server& {
    return *server_;
}

template <typename Service>
auto AsyncServer<Service>::run() -> void {
    void* raw_tag                = {};
    bool  completed_successfully = {};

    while (completion_queue_->Next(&raw_tag, &completed_successfully)) {
        std::lock_guard lock(mutex_);

        auto tag = tagger_.get_tag(raw_tag);
        std::cout << "S: " << (completed_successfully ? "Success: " : "Failure: ") << tag << std::endl;

        if (completed_successfully) {
            switch (tag.label) {

            case ServerTagLabel::NewRpc: {
                auto* rpc = static_cast<detail::AsyncServerRpc<Service>*>(tag.data);
                {
                    auto  new_rpc     = rpc->clone();
                    auto* new_rpc_raw = new_rpc.get();
                    rpc_call_data_.emplace(new_rpc_raw, std::move(new_rpc));
                }
                rpc->invoke_connection_callback();

            } break;

            case ServerTagLabel::Writing: {
            } break;

            case ServerTagLabel::Done: {
                auto* rpc = static_cast<detail::AsyncServerRpc<Service>*>(tag.data);
                rpc->invoke_disconnect_callback();
                rpc_call_data_.erase(rpc);
            } break;

            } // end switch

        } else {
            rpc_call_data_.erase(tag.data);
        }
    }
}

template <typename Service>
auto AsyncServer<Service>::shutdown() -> void {
    std::lock_guard lock(mutex_);
    server_->Shutdown();
    completion_queue_->Shutdown();
}

template <typename Service>
template <typename BaseService, typename Request, typename Response>
auto AsyncServer<Service>::register_rpc(UnaryAsyncRpc<BaseService, Request, Response>             unary_call_ptr,
                                        typename ServerCallbacks<Request, Response>::UnaryConnect on_connect,
                                        DisconnectCallback on_disconnect) -> void {
    std::lock_guard lock(mutex_);

    static_assert(std::is_base_of<BaseService, Service>::value, "BaseService must be a base class of Service");

    auto unary_call_data
        = std::make_unique<detail::AsyncServerUnaryCallData<BaseService, Request, Response>>(tagger_,
                                                                                             *completion_queue_,
                                                                                             std::move(on_disconnect),
                                                                                             service_,
                                                                                             unary_call_ptr,
                                                                                             std::move(on_connect));

    auto raw_unary_call_data = unary_call_data.get();
    rpc_call_data_.emplace(raw_unary_call_data, std::move(unary_call_data));
}

template <typename Service>
template <typename BaseService, typename Request, typename Response>
auto AsyncServer<Service>::register_rpc(ClientStreamAsyncRpc<BaseService, Request, Response> /*call_ptr*/) -> void {
    std::lock_guard lock(mutex_);

    static_assert(std::is_base_of<BaseService, Service>::value, "BaseService must be a base class of Service");
}

template <typename Service>
template <typename BaseService, typename Request, typename Response>
auto AsyncServer<Service>::register_rpc(ServerStreamAsyncRpc<BaseService, Request, Response> /*call_ptr*/) -> void {
    std::lock_guard lock(mutex_);

    static_assert(std::is_base_of<BaseService, Service>::value, "BaseService must be a base class of Service");
}

template <typename Service>
template <typename BaseService, typename Request, typename Response>
auto AsyncServer<Service>::register_rpc(BidirectionalStreamAsyncRpc<BaseService, Request, Response> /*call_ptr*/)
    -> void {
    std::lock_guard lock(mutex_);

    static_assert(std::is_base_of<BaseService, Service>::value, "BaseService must be a base class of Service");
}

} // namespace ltb::net
