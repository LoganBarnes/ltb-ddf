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
#include "async_server_callbacks.hpp"
#include "ltb/net/tagger.hpp"

// external
#include <grpc++/server.h>

// standard
#include <memory>

namespace ltb::net::detail {

template <typename Service>
class AsyncServerRpc {
public:
    explicit AsyncServerRpc(ServerTagger&                tagger,
                            grpc::ServerCompletionQueue& queue,
                            Service&                     service,
                            DisconnectCallback           on_disconnect);
    virtual ~AsyncServerRpc() = default;

    virtual auto clone() -> std::unique_ptr<AsyncServerRpc> = 0;
    virtual auto invoke_connection_callback() -> void       = 0;

    auto invoke_disconnect_callback() -> void;

protected:
    ServerTagger&                tagger_;
    grpc::ServerCompletionQueue& completion_queue_;
    Service&                     service_;
    DisconnectCallback           on_disconnect_;
};

template <typename Service>
AsyncServerRpc<Service>::AsyncServerRpc(ServerTagger&                tagger,
                                        grpc::ServerCompletionQueue& queue,
                                        Service&                     service,
                                        DisconnectCallback           on_disconnect)
    : tagger_(tagger), completion_queue_(queue), service_(service), on_disconnect_(std::move(on_disconnect)) {}

template <typename Service>
auto AsyncServerRpc<Service>::invoke_disconnect_callback() -> void {
    if (on_disconnect_) {
        on_disconnect_(this);
    }
}

} // namespace ltb::net::detail
