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
#include "async_server_unary_writer.hpp"
#include "ltb/net/tagger.hpp"
#include "rpc_function_types.hpp"

namespace ltb::net::detail {

template <typename Service, typename Request, typename Response>
struct AsyncServerUnaryCallData : public AsyncServerRpc<Service> {

    explicit AsyncServerUnaryCallData(ServerTagger&                                             tagger,
                                      grpc::ServerCompletionQueue&                              queue,
                                      DisconnectCallback                                        on_disconnect,
                                      Service&                                                  service,
                                      UnaryAsyncRpc<Service, Request, Response>                 unary_call,
                                      typename ServerCallbacks<Request, Response>::UnaryConnect on_connect);

    ~AsyncServerUnaryCallData() override = default;

    auto clone() -> std::unique_ptr<AsyncServerRpc<Service>> override;
    auto invoke_connection_callback() -> void override;

private:
    UnaryAsyncRpc<Service, Request, Response>                 unary_call_;
    Request                                                   request_;
    std::shared_ptr<ServerAsyncResponseWriter<Response>>      writer_data_;
    typename ServerCallbacks<Request, Response>::UnaryConnect on_connect_;
};

template <typename Service, typename Request, typename Response>
AsyncServerUnaryCallData<Service, Request, Response>::AsyncServerUnaryCallData(
    ServerTagger&                                             tagger,
    grpc::ServerCompletionQueue&                              queue,
    DisconnectCallback                                        on_disconnect,
    Service&                                                  service,
    UnaryAsyncRpc<Service, Request, Response>                 unary_call,
    typename ServerCallbacks<Request, Response>::UnaryConnect on_connect)

    : AsyncServerRpc<Service>(tagger, queue, service, std::move(on_disconnect)),
      unary_call_(unary_call),
      writer_data_(std::make_shared<ServerAsyncResponseWriter<Response>>()),
      on_connect_(std::move(on_connect)) {

    (service.*unary_call)(&writer_data_->context,
                          &request_,
                          &writer_data_->writer,
                          &this->completion_queue_,
                          &this->completion_queue_,
                          this->tagger_.make_tag(this, ServerTagLabel::NewRpc));
}

template <typename Service, typename Request, typename Response>
auto AsyncServerUnaryCallData<Service, Request, Response>::clone() -> std::unique_ptr<AsyncServerRpc<Service>> {
    return std::make_unique<AsyncServerUnaryCallData<Service, Request, Response>>(this->tagger_,
                                                                                  this->completion_queue_,
                                                                                  this->on_disconnect_,
                                                                                  this->service_,
                                                                                  unary_call_,
                                                                                  on_connect_);
}

template <typename Service, typename Request, typename Response>
auto AsyncServerUnaryCallData<Service, Request, Response>::invoke_connection_callback() -> void {
    auto tag = this->tagger_.make_tag(this, ServerTagLabel::Done);
    if (on_connect_) {
        on_connect_(request_, AsyncServerUnaryWriter<Response>{writer_data_, tag});
    } else {
        writer_data_->writer.FinishWithError(grpc::Status{grpc::StatusCode::UNIMPLEMENTED, "RPC not implemented."},
                                             tag);
    }
}

} // namespace ltb::net::detail
