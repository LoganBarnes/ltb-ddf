// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "example_client.hpp"

// project
#include "ltb/gvs/display/gui/imgui_colors.hpp"
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/util/variant_utils.hpp"

// external
#include <grpc++/create_channel.h>

// standard
#include <thread>

using namespace Magnum;

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

ExampleClient::ExampleClient(const Arguments& arguments, std::string const& host_address)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Scene Example")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      async_client_(host_address) {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; // NOLINT(hicpp-signed-bitwise)
    set_callbacks_and_start_client_thread();
}

ExampleClient::ExampleClient(const Arguments& arguments, grpc::Server& interprocess_server)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Scene Example")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      async_client_(interprocess_server) {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; // NOLINT(hicpp-signed-bitwise)
    set_callbacks_and_start_client_thread();
}

ExampleClient::~ExampleClient() {
    async_client_.shutdown();
    client_thread_.join();
}

void ExampleClient::update() {
    while (!update_queue_.empty()) {
        std::visit(ltb::util::Visitor{
                       [this](ltb::util::Error const& error) { error_alert_->record_error(error); },
                       [this](ConnectionState const& state) { connection_state_ = state.state; },
                       [this](DebugLog const& log) { log_stream_ << log.message << "\n"; },
                   },
                   update_queue_.pop_front());
    }
}

void ExampleClient::render(gvs::CameraPackage const& /*camera_package*/) const {
    // Nothing to do
}

void ExampleClient::configure_gui() {
    {
        auto style = ltb::util::make_guard(
            [] {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            },
            [] { ImGui::PopStyleVar(3); });

        auto window_flags = ltb::gvs::set_full_docking_window_settings_and_get_flags();
        ImGui::Begin("MainDockingWindow", nullptr, window_flags);
    }

    auto& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) { // NOLINT(hicpp-signed-bitwise)
        ImGui::DockSpace(ImGui::GetID("FullWindowDockspace"), {0.f, 0.f}, ImGuiDockNodeFlags_PassthruCentralNode);
    }

    if (ImGui::Begin("Server")) {
        /// \todo use the gvs widget for this when it exists
        switch (connection_state_) {
        case net::ClientConnectionState::NoHostSpecified:
            ImGui::Text("State: NoHostSpecified");
            break;
        case net::ClientConnectionState::InterprocessServerAlwaysConnected:
            ImGui::Text("State: InterprocessServerAlwaysConnected");
            break;
        case net::ClientConnectionState::NotConnected:
            ImGui::Text("State: NotConnected");
            break;
        case net::ClientConnectionState::AttemptingToConnect:
            ImGui::Text("State: AttemptingToConnect");
            break;
        case net::ClientConnectionState::Connected:
            ImGui::Text("State: Connected");
            break;
        case net::ClientConnectionState::RecoveringFromFailure:
            ImGui::Text("State: RecoveringFromFailure");
            break;
        case net::ClientConnectionState::Shutdown:
            ImGui::Text("State: Shutdown");
            break;
        }
    }
    ImGui::End();

    if (ImGui::Begin("Messages")) {
        for (const auto& message_id : messages_ids_) {
            std::string user    = "user"; // todo
            auto const& message = messages_data_.at(message_id);
            ImGui::Text("%s: %s", user.c_str(), message.c_str());
        }
        ImGui::Text(">");
        ImGui::SameLine();
        if (ltb::gvs::configure_gui("###current_message", &current_message_, ImGuiInputTextFlags_EnterReturnsTrue)
            && !current_message_.empty()) {

            Action action;

            auto* send_message = action.mutable_send_message();
            send_message->mutable_client_id()->set_value("tmp client id");
            send_message->set_message(current_message_);
            dispatch_action(action);

            current_message_ = "";
        }
    }
    ImGui::End();

    if (ImGui::Begin("Users")) {
    }
    ImGui::End();

    if (ImGui::Begin("Debug")) {
        auto logs = log_stream_.str();
        if (logs.empty()) {
            ImGui::TextColored(ltb::gvs::gray(), "No logs");
        } else {
            ImGui::Text("%s", logs.c_str());
        }
    }
    ImGui::End();

    ImGui::End();
}

auto ExampleClient::resize(Magnum::Vector2i const & /*viewport*/) -> void {}

auto ExampleClient::dispatch_action(Action const& action) -> ExampleClient& {
    async_client_.unary_rpc<util::Result>(
        &ChatRoom::Stub::AsyncDispatchAction,
        action,
        [this](util::Result const& result) {
            std::stringstream stream;
            stream << "DispatchAction response: " << result.ShortDebugString();
            queue_update(DebugLog{stream.str()});
        },
        [this](grpc::Status const& status) {
            std::stringstream stream;
            stream << "DispatchAction status: " << (status.ok() ? "OK" : "ERROR: " + status.error_message());
            queue_update(DebugLog{stream.str()});
        },
        [this](ltb::util::Error const& error) { queue_update(error); });
    return *this;
}

auto ExampleClient::set_callbacks_and_start_client_thread() -> void {
    async_client_.on_state_change(
        [this](auto const& state) {
            std::stringstream stream;
            stream << "State: " << state;
            queue_update(DebugLog{stream.str()});
            queue_update(ConnectionState{state});
        },
        ltb::net::CallImmediately::Yes);
    client_thread_ = std::thread([this] { async_client_.run(); });
}
auto ExampleClient::queue_update(ExampleUpdate update) -> void {
    update_queue_.emplace_back(std::move(update));
    reset_draw_counter();
}

} // namespace ltb::example
