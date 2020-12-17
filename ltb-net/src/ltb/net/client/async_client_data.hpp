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
#include "ltb/util/error.hpp"

// external
#include <grpc++/client_context.h>

// standard
#include <functional>

namespace ltb::net {

enum class ClientConnectionState {
    NoHostSpecified,
    InterprocessServerAlwaysConnected,
    NotConnected,
    AttemptingToConnect,
    Connected,
    RecoveringFromFailure,
    Shutdown,
};

enum class CallImmediately {
    Yes,
    No,
};

using StatusCallback = std::function<void(grpc::Status)>;
using ErrorCallback  = std::function<void(ltb::util::Error)>;
template <typename Response>
using ResponseCallback = std::function<void(Response)>;

struct AsyncClientRpcCallData {
    virtual ~AsyncClientRpcCallData() = default;

    virtual auto process_callbacks() -> void = 0;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;

    StatusCallback status_callback = nullptr;
    ErrorCallback  error_callback  = nullptr;
};

template <typename Response>
struct AsyncClientUnaryCallData : public AsyncClientRpcCallData {
    ~AsyncClientUnaryCallData() override = default;

    auto process_callbacks() -> void override;

    Response response = {};

    std::unique_ptr<grpc_impl::ClientAsyncResponseReader<Response>> response_reader = nullptr;

    ResponseCallback<Response> response_callback = nullptr;
};

template <typename Response>
auto AsyncClientUnaryCallData<Response>::process_callbacks() -> void {
    if (response_callback) {
        response_callback(response);
    }
}

} // namespace ltb::net
