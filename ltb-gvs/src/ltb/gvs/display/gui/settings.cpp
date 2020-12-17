// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
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
#include "settings.hpp"

// project
#include "imgui_utils.hpp"
#include "ltb/util/string.hpp"

// standard
#include <sstream>

namespace ltb::gvs {

using namespace Magnum;

namespace {

auto type_string(const Settings::Camera&) {
    return "Camera";
}

template <typename Enum>
auto to_underlying_type(const Enum& e) {
    return static_cast<std::underlying_type_t<Enum>>(e);
}

template <typename Enum>
auto from_underlying_type(const std::underlying_type_t<Enum>& e) {
    return static_cast<Enum>(e);
}

auto to_string(const Settings::App::MouseMoveEvent::Modifiers& modifiers) -> std::string {
    std::stringstream result;
    if (modifiers & Settings::App::InputEvent::Modifier::Shift) {
        result << to_underlying_type(Settings::App::InputEvent::Modifier::Shift) << ",";
    }
    if (modifiers & Settings::App::InputEvent::Modifier::Ctrl) {
        result << to_underlying_type(Settings::App::InputEvent::Modifier::Ctrl) << ",";
    }
    if (modifiers & Settings::App::InputEvent::Modifier::Alt) {
        result << to_underlying_type(Settings::App::InputEvent::Modifier::Alt) << ",";
    }
    if (modifiers & Settings::App::InputEvent::Modifier::Super) {
        result << to_underlying_type(Settings::App::InputEvent::Modifier::Super) << ",";
    }
    return result.str();
}

auto get_button(const std::string& button_str) {
    std::underlying_type_t<Settings::App::MouseMoveEvent::Button> val;
    std::istringstream                                            iss(button_str);
    iss >> val;
    return static_cast<Settings::App::MouseMoveEvent::Button>(val);
}

auto get_modifier(const std::string& modifier_str) {
    std::underlying_type_t<Settings::App::MouseMoveEvent::Modifier> val;
    std::istringstream                                              iss(modifier_str);
    iss >> val;
    return static_cast<Settings::App::MouseMoveEvent::Modifier>(val);
}

auto get_modifiers(const std::string& modifiers_str) {
    Settings::App::MouseMoveEvent::Modifiers modifiers;
    std::istringstream                       iss(modifiers_str);

    std::string modifier_str;
    while (std::getline(iss, modifier_str, ',')) {
        modifiers |= get_modifier(modifier_str);
    }

    return modifiers;
}

auto settings_read_open(ImGuiContext* /*ctx*/, ImGuiSettingsHandler* handler, const char* name) -> void* {
    auto name_str = std::string(name);

    auto* settings = static_cast<Settings*>(handler->UserData);

    if (name_str == type_string(settings->camera)) {
        return &settings->camera;
    }

    return handler->UserData;
}

void set_camera(Settings::Camera* camera_settings, const std::string& line) {
    assert(camera_settings);

    if (util::starts_with(line, "select_button=")) {
        camera_settings->select_button = get_button(line.substr(std::string("select_button=").size()));

    } else if (util::starts_with(line, "select_modifiers=")) {
        camera_settings->select_modifiers = get_modifiers(line.substr(std::string("select_modifiers=").size()));

    } else if (util::starts_with(line, "pan_button=")) {
        camera_settings->pan_button = get_button(line.substr(std::string("pan_button=").size()));

    } else if (util::starts_with(line, "pan_modifiers=")) {
        camera_settings->pan_modifiers = get_modifiers(line.substr(std::string("pan_modifiers=").size()));

    } else if (util::starts_with(line, "orbit_button=")) {
        camera_settings->orbit_button = get_button(line.substr(std::string("orbit_button=").size()));

    } else if (util::starts_with(line, "orbit_modifiers=")) {
        camera_settings->orbit_modifiers = get_modifiers(line.substr(std::string("orbit_modifiers=").size()));
    }
}

void settings_read_line(ImGuiContext* /*ctx*/, ImGuiSettingsHandler* handler, void* entry, const char* line) {
    auto line_str = std::string(line);

    if (line_str.empty()) {
        return;
    }

    auto* settings = static_cast<Settings*>(handler->UserData);

    if (entry == &settings->camera) {
        set_camera(&settings->camera, line);
    }
}

void settings_write_all(ImGuiContext* /*ctx*/, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf) {

    const auto* settings        = static_cast<Settings*>(handler->UserData);
    const auto& camera_settings = settings->camera;

    std::stringstream ss;
    ss << "[" << handler->TypeName << "][Camera]\n"
       << "select_button=" << to_underlying_type(camera_settings.select_button) << "\n"
       << "select_modifiers=" << to_string(camera_settings.select_modifiers) << "\n"
       << "pan_button=" << to_underlying_type(camera_settings.pan_button) << "\n"
       << "pan_modifiers=" << to_string(camera_settings.pan_modifiers) << "\n"
       << "orbit_button=" << to_underlying_type(camera_settings.orbit_button) << "\n"
       << "orbit_modifiers=" << to_string(camera_settings.orbit_modifiers) << "\n"
       << std::endl;

    auto settings_str = ss.str();

    out_buf->reserve(out_buf->size() + static_cast<int>(settings_str.size())); // ballpark reserve
    out_buf->appendf("%s\n", settings_str.c_str());
}

} // namespace

Settings::Settings() {
    ini_handler_.TypeName   = "UserSettings";
    ini_handler_.TypeHash   = ImHashStr("UserSettings");
    ini_handler_.ReadOpenFn = settings_read_open;
    ini_handler_.ReadLineFn = settings_read_line;
    ini_handler_.WriteAllFn = settings_write_all;
    ini_handler_.UserData   = this;
}

ImGuiSettingsHandler Settings::handler() {
    return ini_handler_;
}

void Settings::configure_gui() {
    if (ImGui::CollapsingHeader("Camera Settings")) {

        auto single_mouse_radio_button = [](const auto& label, auto button, auto button_type) {
            if (ImGui::RadioButton(label, button == button_type)) {
                button = button_type;
            }
            return button;
        };

        auto mouse_radio_buttons = [&single_mouse_radio_button](const auto& label, auto button) {
            ScopedID     id(label);
            ScopedIndent indent;

            ImGui::TextUnformatted(label);
            ImGui::SameLine();
            button = single_mouse_radio_button("Left", button, App::MouseMoveEvent::Button::Left);
            ImGui::SameLine();
            button = single_mouse_radio_button("Middle", button, App::MouseMoveEvent::Button::Middle);
            ImGui::SameLine();
            button = single_mouse_radio_button("Right", button, App::MouseMoveEvent::Button::Right);
            return button;
        };

        ImGui::TextUnformatted("Mouse Buttons");
        camera.select_button = mouse_radio_buttons("Selection: ", camera.select_button);
        camera.pan_button    = mouse_radio_buttons("Panning:   ", camera.pan_button);
        camera.orbit_button  = mouse_radio_buttons("Rotation:  ", camera.orbit_button);
    }
}

} // namespace ltb::gvs
