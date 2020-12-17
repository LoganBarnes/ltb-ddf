// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2020 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/gvs/display/gui/imgui_magnum_application.hpp"
#include "ltb/net/client/async_client.hpp"
#include "ltb/util/blocking_queue.hpp"

// generated
#include <protos/chat/chat_room.grpc.pb.h>

// standard
#include <sstream>
#include <variant>

namespace ltb::example {

struct ConnectionState {
    net::ClientConnectionState state;
};
struct DebugLog {
    std::string message;
};
using ExampleUpdate = std::variant<ltb::util::Error, ConnectionState, DebugLog>;

class ExampleClient : public gvs::ImGuiMagnumApplication {
public:
    explicit ExampleClient(const Arguments& arguments, std::string const& host_address);
    explicit ExampleClient(const Arguments& arguments, grpc::Server& interprocess_server);
    ~ExampleClient() override;

    auto dispatch_action(Action const& action) -> ExampleClient&;

private:
    auto update() -> void override;
    auto render(gvs::CameraPackage const& camera_package) const -> void override;
    auto configure_gui() -> void override;

    auto resize(Magnum::Vector2i const& viewport) -> void override;

    std::thread                     client_thread_;
    ltb::net::AsyncClient<ChatRoom> async_client_;

    std::vector<std::string>                     messages_ids_;
    std::unordered_map<std::string, std::string> messages_data_;

    std::string current_message_;

    std::stringstream               log_stream_;
    ltb::net::ClientConnectionState connection_state_ = ltb::net::ClientConnectionState::NoHostSpecified;

    ltb::util::BlockingQueue<ExampleUpdate> update_queue_;

    auto set_callbacks_and_start_client_thread() -> void;
    auto queue_update(ExampleUpdate update) -> void;
};

} // namespace ltb::example
