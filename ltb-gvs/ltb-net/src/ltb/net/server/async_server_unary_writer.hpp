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

// external
#include <grpc++/server_context.h>

// standard
#include <functional>

namespace ltb::net {

using ClientID = void*;

namespace detail {

template <typename Response>
struct AsyncServerUnaryWriterData {
    virtual ~AsyncServerUnaryWriterData()                                          = 0;
    virtual auto cancel() -> void                                                  = 0;
    virtual auto finish(Response response, grpc::Status status, void* tag) -> void = 0;
};

template <typename Response>
AsyncServerUnaryWriterData<Response>::~AsyncServerUnaryWriterData() = default;

template <typename Response, typename Writer>
struct TypedAsyncServerUnaryWriterData : public AsyncServerUnaryWriterData<Response> {
    explicit TypedAsyncServerUnaryWriterData() : writer(&context) {}
    ~TypedAsyncServerUnaryWriterData() override = default;

    auto cancel() -> void override { context.TryCancel(); }
    auto finish(Response response, grpc::Status status, void* tag) -> void override {
        writer.Finish(response, status, tag);
    }

    grpc::ServerContext context;
    Writer              writer;
    // ^  grpc_impl::ServerAsyncResponseWriter<Response>
    // or grpc_impl::ServerAsyncReader<Response, Request>
};

template <typename Response>
using ServerAsyncResponseWriter
    = TypedAsyncServerUnaryWriterData<Response, grpc_impl::ServerAsyncResponseWriter<Response>>;

template <typename Response, typename Request>
using ServerAsyncReader = TypedAsyncServerUnaryWriterData<Response, grpc_impl::ServerAsyncReader<Response, Request>>;

} // namespace detail

template <typename Response>
struct AsyncServerUnaryWriter {
public:
    explicit AsyncServerUnaryWriter(std::weak_ptr<detail::AsyncServerUnaryWriterData<Response>> data, void* tag);

    auto               cancel() -> void;
    auto               finish(Response response, grpc::Status status) -> void;
    [[nodiscard]] auto client_id() const -> ClientID const&;

private:
    std::weak_ptr<detail::AsyncServerUnaryWriterData<Response>> data_;
    void*                                                       tag_;
};

template <typename Response>
AsyncServerUnaryWriter<Response>::AsyncServerUnaryWriter(
    std::weak_ptr<detail::AsyncServerUnaryWriterData<Response>> data, void* tag)
    : data_(std::move(data)), tag_(tag) {}

template <typename Response>
auto AsyncServerUnaryWriter<Response>::cancel() -> void {
    if (auto data = data_.lock()) {
        data->cancel();
    }
}

template <typename Response>
auto AsyncServerUnaryWriter<Response>::finish(Response response, grpc::Status status) -> void {
    if (auto data = data_.lock()) {
        data->finish(response, status, tag_);
    }
}

template <typename Response>
auto AsyncServerUnaryWriter<Response>::client_id() const -> ClientID const& {
    return tag_;
}

} // namespace ltb::net
