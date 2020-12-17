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

// grpcw
#include "ltb/grpcw/client/grpc_client_state.hpp"
#include "ltb/grpcw/client/grpc_client_stream.hpp"
#include "ltb/grpcw/forward_declarations.hpp"
#include "ltb/util/atomic_data.hpp"

// third-party
#include <grpc++/channel.h>
#include <grpc++/completion_queue.h>
#include <grpc++/create_channel.h>
#include <grpc++/server.h>

// standard
#include <thread>
#include <unordered_map>

namespace grpcw {
namespace client {

inline GrpcClientState to_cnc_client_state(grpc_connectivity_state state) {
    switch (state) {
    case GRPC_CHANNEL_SHUTDOWN:
    case GRPC_CHANNEL_IDLE:
        return GrpcClientState::not_connected;

    case GRPC_CHANNEL_CONNECTING:
    case GRPC_CHANNEL_TRANSIENT_FAILURE:
        return GrpcClientState::attempting_to_connect;

    case GRPC_CHANNEL_READY:
        return GrpcClientState::connected;
    }

    throw std::invalid_argument("Invalid grpc_connectivity_state");
}

auto default_channel_arguments() -> grpc::ChannelArguments;

template <typename Service>
class GrpcClient {
    template <typename Result>
    using StreamInitFunc = typename GrpcClientStream<Service, Result>::InitFunc;

    template <typename Result>
    using StreamOnUpdate = typename GrpcClientStream<Service, Result>::OnUpdate;

    template <typename Result>
    using StreamOnFinish = typename GrpcClientStream<Service, Result>::OnFinish;

    using ConnectionChangeCallback = std::function<void(GrpcClientState)>;

    template <typename Result>
    class GrpcClientStreamCallbackSetter;

public:
    explicit GrpcClient() = default;
    ~GrpcClient();

    ///
    /// \brief Attempt to connect to a host address in the form "server:port".
    ///
    /// The client will attempt to connect and as the connection state changes the
    /// 'connection_change_callback' will be invoked to inform the user of the changes.
    ///
    /// NOTE: The callback will be invoked from a separate thread.
    ///
    void change_server(const std::string& address, ConnectionChangeCallback connection_change_callback);

    ///
    /// \brief Create a direct channel to a server running in the same process.
    ///
    /// No connection callback is needed because the client will always be in a connected state
    ///
    void change_server(std::shared_ptr<grpc::Channel> in_process_channel);

    ///
    /// \brief Add an RPC call that will return a stream of data
    ///
    template <typename Result>
    GrpcClientStreamCallbackSetter<Result> register_stream(StreamInitFunc<Result> init_func);

    ///
    /// \brief Stop all connection attempts or disconnect (if already connected)
    ///
    void kill_streams_and_channel();

    const std::string& get_server_address() const;
    GrpcClientState    get_state();
    bool               is_using_in_process_server() const;

    ///
    /// \brief Safely use the service stub to make RPC calls.
    ///
    /// If the client is not connected this function will return false and 'usage_func' will not be invoked
    ///
    template <typename UsageFunc>
    bool use_stub(const UsageFunc& usage_func);

private:
    /// \brief All the data shared between threads
    struct SharedData {
        grpc_connectivity_state                 connection_state = GRPC_CHANNEL_IDLE;
        std::shared_ptr<grpc::Channel>          channel          = nullptr;
        std::unique_ptr<typename Service::Stub> stub             = nullptr;
        std::unordered_map<void*, std::unique_ptr<GrpcClientStreamInterface<Service>>> streams;
    };

    /// Use atomic access to manipulate the shared data
    ltb::util::AtomicData<SharedData> shared_data_;

    bool        using_in_process_server_ = false;
    std::string server_address_;

    /// Used as a label in 'queue_' so we know when the state changed.
    void* connection_change_tag = reinterpret_cast<void*>(0x1);

    // These are pointers so they can be reused after calling 'Shutdown' and 'join' respectively.
    std::unique_ptr<grpc::CompletionQueue> queue_;
    std::unique_ptr<std::thread>           run_thread_;

    void run(const std::function<void(const GrpcClientState&)>& connection_change_callback);

    /// \brief Sets the `OnUpdate` callback for the given stream
    template <typename Result>
    void on_stream_update(void* key, StreamOnUpdate<Result> on_update);

    /// \brief Sets the `OnFinish` callback for the given stream
    template <typename Result>
    void on_stream_finish(void* key, StreamOnFinish<Result> on_finish);

    /// \brief Allows callbacks to be set for `GrpcClientStream`s
    /// \tparam Result is the stream's result type
    template <typename Result>
    class GrpcClientStreamCallbackSetter {
    public:
        GrpcClientStreamCallbackSetter(GrpcClient<Service>& client, void* stream) : client_(client), stream_(stream) {}

        /// \brief Set the OnUpdate callback for this stream
        GrpcClientStreamCallbackSetter<Result>& on_update(StreamOnUpdate<Result> on_update) {
            client_.on_stream_update<Result>(stream_, std::move(on_update));
            return *this;
        }

        /// \brief Set the OnFinish callback for this stream
        GrpcClientStreamCallbackSetter<Result>& on_finish(StreamOnFinish<Result> on_finish) {
            client_.on_stream_finish<Result>(stream_, std::move(on_finish));
            return *this;
        }

    private:
        GrpcClient<Service>& client_;
        void*                stream_;
    };
};

template <typename Service>
GrpcClient<Service>::~GrpcClient() {
    kill_streams_and_channel();
}

template <typename Service>
void GrpcClient<Service>::change_server(const std::string&       address,
                                        ConnectionChangeCallback connection_change_callback) {
    // Disconnect from the previous server
    kill_streams_and_channel();

    // Set the non-shared data
    using_in_process_server_ = false;
    server_address_          = address;
    queue_                   = std::make_unique<grpc::CompletionQueue>();

    bool            state_changed = false;
    GrpcClientState cnc_client_state;

    // Update the shared data
    shared_data_.use_safely([&](SharedData& data) {
        // Create new channel to establish a connection
        data.channel = grpc::CreateCustomChannel(server_address_,
                                                 grpc::InsecureChannelCredentials(),
                                                 client::default_channel_arguments());
        data.stub    = Service::NewStub(data.channel);

        // Get the current connection state and check if it has changed
        auto new_state = data.channel->GetState(true);

        cnc_client_state = to_cnc_client_state(new_state);
        state_changed    = (cnc_client_state != to_cnc_client_state(data.connection_state));

        data.connection_state = new_state;

        // Ask the channel to notify us when state changes by updating 'queue_'
        auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(15);
        data.channel->NotifyOnStateChange(data.connection_state, deadline, queue_.get(), connection_change_tag);
    });

    if (state_changed) {
        connection_change_callback(cnc_client_state);
    }

    // Wait for state changes
    run_thread_ = std::make_unique<std::thread>(&GrpcClient<Service>::run, this, std::move(connection_change_callback));
}

template <typename Service>
void GrpcClient<Service>::change_server(std::shared_ptr<grpc::Channel> in_process_channel) {
    // Disconnect from the previous server
    kill_streams_and_channel();

    using_in_process_server_ = true;
    server_address_          = "In-Process";

    shared_data_.use_safely([in_process_channel = std::move(in_process_channel)](SharedData& data) {
        data.channel = in_process_channel;
        data.stub    = Service::NewStub(data.channel);

        // No connectivity updates need to happen because the server is running in the same process
        data.connection_state = GRPC_CHANNEL_READY;
    });
}

template <typename Service>
template <typename Result>
auto GrpcClient<Service>::register_stream(StreamInitFunc<Result> init_func) -> GrpcClientStreamCallbackSetter<Result> {
    void* key;

    shared_data_.use_safely([init_func = std::move(init_func), &key](SharedData& data) {
        auto stream = std::make_unique<GrpcClientStream<Service, Result>>(std::move(init_func));

        // Start the stream if the channel is already connected
        if (data.channel and data.connection_state == GRPC_CHANNEL_READY) {
            stream->start_stream(*data.stub);
        }

        key = stream.get();
        data.streams.emplace(key, std::move(stream));
    });

    return {*this, key};
}

template <typename Service>
void GrpcClient<Service>::kill_streams_and_channel() {
    shared_data_.use_safely([](SharedData& data) {
        for (auto& stream_pair : data.streams) {
            stream_pair.second->stop_stream();
        }

        // Delete the stub and channel first to trigger the shutdown events in the channel.
        // ('data.stub' has a shared pointer to channel so it needs to be deleted too)
        data.stub    = nullptr;
        data.channel = nullptr;
    });

    // Tell the queue to exit once all its current items have been popped
    if (queue_) {
        queue_->Shutdown();
    }

    // Wait for the thread to process the remaining events and exit
    if (run_thread_ and run_thread_->joinable()) {
        run_thread_->join();
    }
}

template <typename Service>
const std::string& GrpcClient<Service>::get_server_address() const {
    return server_address_;
}

template <typename Service>
GrpcClientState GrpcClient<Service>::get_state() {
    grpc_connectivity_state state_copy;
    shared_data_.use_safely([&](const SharedData& data) { state_copy = data.connection_state; });
    return to_cnc_client_state(state_copy);
}

template <typename Service>
bool GrpcClient<Service>::is_using_in_process_server() const {
    return using_in_process_server_;
}

template <typename Service>
template <typename UsageFunc>
bool GrpcClient<Service>::use_stub(const UsageFunc& usage_func) {
    bool stub_valid = false;

    shared_data_.use_safely([&](const SharedData& data) {
        if (data.connection_state == GRPC_CHANNEL_READY) {
            stub_valid = true;
            usage_func(*data.stub);
        }
    });

    return stub_valid;
}

// This function is run from the 'run_thread_' thread
template <typename Service>
void GrpcClient<Service>::run(const std::function<void(const GrpcClientState&)>& connection_change_callback) {

    GrpcClientState cnc_client_state;

    void* current_tag; // A label so se can identify the current update
    bool  result_ok; // Set to false if the queue receives updates due to cancellation (like hitting our deadline)

    while (queue_->Next(&current_tag, &result_ok)) {

        // The queue is currently only set up to receive connection changes
        assert(current_tag == connection_change_tag);

        bool state_changed = false;

        shared_data_.use_safely([&](SharedData& data) {
            if (data.channel) { // not shutdown yet

                if (result_ok) {
                    auto old_client_state = to_cnc_client_state(data.connection_state);
                    data.connection_state = data.channel->GetState(true);
                    cnc_client_state      = to_cnc_client_state(data.connection_state);

                    state_changed = (cnc_client_state != old_client_state);

                    if (state_changed) {
                        if (data.connection_state == GRPC_CHANNEL_READY) {
                            for (auto& stream_pair : data.streams) {
                                stream_pair.second->start_stream(*data.stub);
                            }
                        } else {
                            for (auto& stream_pair : data.streams) {
                                stream_pair.second->stop_stream();
                            }
                        }
                    }
                }

                // Tell the channel to keep sending connection updates using this same queue
                auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::seconds(60);
                data.channel->NotifyOnStateChange(data.connection_state, deadline, queue_.get(), connection_change_tag);

            } else if (to_cnc_client_state(data.connection_state) != GrpcClientState::not_connected) {
                // The channel has been shutdown but the state is not set so do not disconnected yet.
                // Set it appropriately.
                data.connection_state = GRPC_CHANNEL_SHUTDOWN;
                cnc_client_state      = to_cnc_client_state(data.connection_state);
                state_changed         = true;
            }
        });

        // Do the callback outside the locked code to prevent the user from deadlocking the program.
        if (state_changed) {
            connection_change_callback(cnc_client_state);
        }
    }
}

template <typename Service>
template <typename Result>
void GrpcClient<Service>::on_stream_update(void* key, StreamOnUpdate<Result> on_update) {
    shared_data_.use_safely([on_update = std::move(on_update), &key](SharedData& data) {
        auto iter = data.streams.find(key);
        if (iter == data.streams.end()) {
            return;
        }
        auto* stream = dynamic_cast<GrpcClientStream<Service, Result>*>(iter->second.get());
        assert(stream);

        stream->on_update(std::move(on_update));
    });
}

template <typename Service>
template <typename Result>
void GrpcClient<Service>::on_stream_finish(void* key, StreamOnFinish<Result> on_finish) {
    shared_data_.use_safely([on_finish = std::move(on_finish), &key](SharedData& data) {
        auto iter = data.streams.find(key);
        if (iter == data.streams.end()) {
            return;
        }
        auto* stream = dynamic_cast<GrpcClientStream<Service, Result>*>(iter->second.get());
        assert(stream);

        stream->on_finish(std::move(on_finish));
    });
}

} // namespace client
} // namespace grpcw
