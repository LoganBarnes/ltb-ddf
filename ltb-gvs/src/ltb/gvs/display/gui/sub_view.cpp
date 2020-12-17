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
#include "sub_view.hpp"

// project
#include "imgui_utils.hpp"

namespace ltb::gvs {

SubView::SubView() = default;

auto SubView::settings() const -> SubViewSettings {
    return {};
}

auto SubView::display() -> void {
    auto open = this->open();

    if (open) {
        if (this->check_and_cancel_focus_request()) {
            ImGui::SetNextWindowFocus();
        }

        auto title = this->title();

        auto settings = this->settings();
        ImGui::SetNextWindowSizeConstraints(settings.min_size_when_not_docked, settings.max_size_when_not_docked);
        if (ImGui::Begin(title.c_str(), &open)) {
            ScopedIndent indent{};
            this->set_visible(true);
            this->configure_gui();
        } else {
            this->set_visible(false);
        }
        ImGui::End();
        this->set_open(open);
    }
}

auto SubView::check_and_cancel_focus_request() -> bool {
    auto request_focus = request_focus_;
    request_focus_     = false;
    return request_focus;
}

auto SubView::open() const -> bool {
    return open_;
}

auto SubView::visible() const -> bool {
    return visible_;
}

auto SubView::set_open(bool open) -> void {
    open_ = open;
    set_visible(visible_);
}

auto SubView::set_visible(bool visible) -> void {
    visible_ = visible && open_;
}

} // namespace ltb::gvs
