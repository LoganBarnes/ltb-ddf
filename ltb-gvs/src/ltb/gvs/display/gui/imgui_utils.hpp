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
#include "ltb/util/generic_guard.hpp"

// external
#include <imgui.h>

// standard
#include <string>

namespace ltb::gvs {

struct Disable {
    class Guard {
    public:
        explicit Guard(bool disable);
        ~Guard();

    private:
        bool disable_;
    };

    static void disable_push();
    static void disable_pop();
};

auto configure_gui(const std::string&          label,
                   std::string*                data,
                   ImGuiInputTextFlags_ const& flags = ImGuiInputTextFlags_None) -> bool;

class ScopedIndent {
public:
    explicit ScopedIndent(float indent_w = 0.f)
        : guard_(util::make_guard(&ImGui::Indent, &ImGui::Unindent, indent_w)) {}

private:
    util::GenericGuard<decltype(&ImGui::Indent), decltype(&ImGui::Unindent), float&> guard_;
};

class ScopedID {
public:
    explicit ScopedID(const char* str_id);
    explicit ScopedID(const char* str_id_begin, const char* str_id_end);
    explicit ScopedID(const void* ptr_id);
    explicit ScopedID(int int_id);
    ~ScopedID();
};

auto display_fps_info() -> void;
auto display_device_info() -> void;

auto add_three_line_separator() -> void;

auto set_full_docking_window_settings_and_get_flags() -> int;

} // namespace ltb::gvs
