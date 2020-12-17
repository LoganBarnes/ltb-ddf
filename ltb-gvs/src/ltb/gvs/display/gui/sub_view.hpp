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

// external
#include <imgui.h>

// standard
#include <limits>
#include <string>

namespace ltb::gvs {

struct SubViewSettings {
    ImVec2 min_size_when_not_docked = {0, 0};
    ImVec2 max_size_when_not_docked = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
};

class SubView {
public:
    explicit SubView();
    virtual ~SubView() = default;

    virtual auto title() -> std::string  = 0;
    virtual auto configure_gui() -> void = 0;

    [[nodiscard]] virtual auto settings() const -> SubViewSettings;
    virtual auto               display() -> void;

    auto check_and_cancel_focus_request() -> bool;

    [[nodiscard]] auto open() const -> bool;
    [[nodiscard]] auto visible() const -> bool;

    auto set_open(bool open) -> void;
    auto set_visible(bool visible) -> void;

protected:
    bool request_focus_ = false;
    bool open_          = true;
    bool visible_       = false;
};

} // namespace ltb::gvs
