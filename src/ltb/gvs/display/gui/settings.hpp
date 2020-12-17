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
#pragma once

// project
#include "ltb/gvs/forward_declarations.hpp"

// external
#include <Magnum/Platform/GlfwApplication.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace ltb::gvs {

struct Settings {
    using App = Magnum::Platform::Application;

    struct Camera {
        App::MouseMoveEvent::Button    select_button    = App::MouseMoveEvent::Button::Left;
        App::MouseMoveEvent::Modifiers select_modifiers = {};
        App::MouseMoveEvent::Button    pan_button       = App::MouseMoveEvent::Button::Middle;
        App::MouseMoveEvent::Modifiers pan_modifiers    = {};
        App::MouseMoveEvent::Button    orbit_button     = App::MouseMoveEvent::Button::Right;
        App::MouseMoveEvent::Modifiers orbit_modifiers  = {};
    } camera;

    Settings();

    ImGuiSettingsHandler handler();

    void configure_gui();

private:
    // Add .ini handle for UserData type
    ImGuiSettingsHandler ini_handler_;
};

} // namespace ltb::gvs
