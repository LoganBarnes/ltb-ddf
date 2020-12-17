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
#include "example_client.hpp"

// project
#include "ltb/net/client/async_client.hpp"

// external
#include <grpc++/create_channel.h>

// standard
#include <thread>

namespace ltb::example {
namespace {

auto operator<<(std::ostream& os, ltb::net::ClientConnectionState const& state) -> std::ostream& {
    switch (state) {
    case net::ClientConnectionState::NoHostSpecified:
        return os << "NoHostSpecified";
    case net::ClientConnectionState::InterprocessServerAlwaysConnected:
        return os << "InterprocessServerAlwaysConnected";
    case ltb::net::ClientConnectionState::NotConnected:
        return os << "NotConnected";
    case ltb::net::ClientConnectionState::AttemptingToConnect:
        return os << "AttemptingToConnect";
    case ltb::net::ClientConnectionState::Connected:
        return os << "Connected";
    case ltb::net::ClientConnectionState::RecoveringFromFailure:
        return os << "RecoveringFromFailure";
    case ltb::net::ClientConnectionState::Shutdown:
        return os << "Shutdown";
    }
    throw std::invalid_argument("Invalid grpc_connectivity_state type");
}

} // namespace

ExampleClient::ExampleClient(std::string const& host_address) : async_client_(host_address) {}

ExampleClient::ExampleClient(grpc::Server& interprocess_server) : async_client_(interprocess_server) {}

auto ExampleClient::run() -> void {

    async_client_.on_state_change([](auto state) { std::cout << state << std::endl; }, ltb::net::CallImmediately::Yes);

    std::thread run_thread([this] { async_client_.run(); });

    std::cout << "EC: Press enter to send message" << std::endl;
    std::cin.ignore();
    std::cout << "EC: Sending message..." << std::endl;

    Action action;
    action.mutable_send_message()->mutable_client_id()->set_value("Tmp_client_id");
    action.mutable_send_message()->set_message("Test message");
    dispatch_action(action);

    std::cout << "EC: Press enter to shut down" << std::endl;
    std::cin.ignore();
    std::cout << "EC: Shutting down..." << std::endl;
    async_client_.shutdown();

    std::cout << "EC: Shutdown" << std::endl;
    run_thread.join();
    std::cout << "EC: Exit" << std::endl;
}

auto ExampleClient::dispatch_action(Action const& action) -> ExampleClient& {
    async_client_.unary_rpc<util::Result>(
        &ChatRoom::Stub::AsyncDispatchAction,
        action,
        [](util::Result result) { std::cout << "DispatchAction response: " << result.ShortDebugString() << std::endl; },
        [](grpc::Status status) {
            std::cout << "DispatchAction status: " << (status.ok() ? "OK" : "ERROR: " + status.error_message())
                      << std::endl;
        },
        [](ltb::util::Error error) { std::cout << "DispatchAction error: " << error.error_message() << std::endl; });
    return *this;
}

} // namespace ltb::example
