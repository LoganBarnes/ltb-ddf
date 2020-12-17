// ///////////////////////////////////////////////////////////////////////////////////////
// gRPC Wrapper
// Copyright (c) 2019 Logan Barnes - All Rights Reserved
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

// third-party
#include <grpc++/client_context.h>

// standard
#include <functional>
#include <thread>

namespace grpcw {
namespace client {

template <typename Service>
class GrpcClientStreamInterface {
public:
    virtual ~GrpcClientStreamInterface()                    = 0;
    virtual void start_stream(typename Service::Stub& stub) = 0;
    virtual void stop_stream()                              = 0;
    virtual bool streaming()                                = 0;
};

template <typename Service>
GrpcClientStreamInterface<Service>::~GrpcClientStreamInterface() = default;

template <typename Service, typename Return>
class GrpcClientStream : public GrpcClientStreamInterface<Service> {
public:
    using InitFunc = std::function<std::unique_ptr<grpc_impl::ClientReader<Return>>(typename Service::Stub&,
                                                                                    grpc::ClientContext*)>;
    using OnUpdate = std::function<void(Return&&)>;
    using OnFinish = std::function<void(const grpc::Status&)>;

    explicit GrpcClientStream(InitFunc init_func);
    ~GrpcClientStream() = default;

    void start_stream(typename Service::Stub& stub) override;
    void stop_stream() override;
    bool streaming() override;

    GrpcClientStream<Service, Return>& on_update(OnUpdate on_update);
    GrpcClientStream<Service, Return>& on_finish(OnFinish on_finish);

private:
    std::unique_ptr<grpc::ClientContext>             context_       = nullptr;
    std::unique_ptr<grpc_impl::ClientReader<Return>> reader_        = nullptr;
    std::unique_ptr<std::thread>                     stream_thread_ = nullptr;

    InitFunc init_func_;
    OnUpdate on_update_;
    OnFinish on_finish_;
};

template <typename Service, typename Return>
GrpcClientStream<Service, Return>::GrpcClientStream(InitFunc init_func) : init_func_(init_func) {}

template <typename Service, typename Return>
void GrpcClientStream<Service, Return>::start_stream(typename Service::Stub& stub) {
    if (streaming()) {
        return;
    }

    context_ = std::make_unique<grpc::ClientContext>();
    reader_  = init_func_(stub, context_.get());

    stream_thread_ = std::make_unique<std::thread>([this] {
        Return update;

        while (reader_->Read(&update)) {
            if (on_update_) {
                on_update_(std::move(update));
            }
        }

        grpc::Status status = reader_->Finish();
        if (on_finish_) {
            on_finish_(status);
        }
    });
}

template <typename Service, typename Return>
void GrpcClientStream<Service, Return>::stop_stream() {
    if (not streaming()) {
        return;
    }

    context_->TryCancel();
    stream_thread_->join();
    context_ = nullptr;
}

template <typename Service, typename Return>
bool GrpcClientStream<Service, Return>::streaming() {
    return context_ != nullptr;
}

template <typename Service, typename Return>
GrpcClientStream<Service, Return>& GrpcClientStream<Service, Return>::on_update(OnUpdate on_update) {
    on_update_ = std::move(on_update);
    return *this;
}

template <typename Service, typename Return>
GrpcClientStream<Service, Return>& GrpcClientStream<Service, Return>::on_finish(OnFinish on_finish) {
    on_finish_ = std::move(on_finish);
    return *this;
}

} // namespace client
} // namespace grpcw
