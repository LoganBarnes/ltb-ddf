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
#include "async_client.hpp"

namespace ltb::net::detail {

auto to_client_connection_state(grpc_connectivity_state const& state) -> ClientConnectionState {
    switch (state) {
    case GRPC_CHANNEL_IDLE:
        return ClientConnectionState::NotConnected;
    case GRPC_CHANNEL_CONNECTING:
        return ClientConnectionState::AttemptingToConnect;
    case GRPC_CHANNEL_READY:
        return ClientConnectionState::Connected;
    case GRPC_CHANNEL_TRANSIENT_FAILURE:
        return ClientConnectionState::RecoveringFromFailure;
    case GRPC_CHANNEL_SHUTDOWN:
        return ClientConnectionState::Shutdown;
    }

    throw std::invalid_argument("Invalid grpc_connectivity_state");
}

auto state_notification_deadline() -> std::chrono::time_point<std::chrono::system_clock> {
    return std::chrono::time_point<std::chrono::system_clock>::max();
    // return std::chrono::system_clock::now() + std::chrono::seconds(60);
}

} // namespace ltb::net::detail
