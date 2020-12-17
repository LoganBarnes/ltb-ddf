#include <utility>

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
#include "display_window.hpp"

// project
#include "backends/opengl_backend.hpp"
#include "display_scene.hpp"
#include "gui/imgui_theme.hpp"
#include "gui/imgui_utils.hpp"
#include "gui/scene_gui.hpp"

// external
#include <Magnum/GL/Context.h>
#include <imgui.h>

// standard
#include <limits>

using namespace Magnum;

namespace ltb::gvs {
namespace {

int   fake_argc    = 1;
char  fake_arg[4]  = {'t', 'e', 's', 't'};
char* fake_argv[1] = {fake_arg};

} // namespace

DisplayWindow::DisplayWindow(DisplayScene& parent_scene)
    : ImGuiMagnumApplication(Arguments{fake_argc, fake_argv},
                             Configuration{}
                                 .setTitle("Geometry Visualisation Client")
                                 .setSize({1280, 720})
                                 .setWindowFlags(Configuration::WindowFlag::Resizable)),
      parent_scene_(parent_scene),
      scene_backend_(std::make_unique<OpenglBackend>()) {}

DisplayWindow::~DisplayWindow() = default;

auto DisplayWindow::thread_safe_update(SceneUpdateFunc update_func) -> void {
    std::lock_guard lock(update_lock_);
    update_functions_.emplace_back(std::move(update_func));
    this->reset_draw_counter();
}

void DisplayWindow::update() {
    std::lock_guard lock(update_lock_);

    // Process an update
    constexpr auto max_updates_per_step = 10u;
    for (auto update = 0u; !update_functions_.empty() && update < max_updates_per_step; ++update) {
        auto update_func = std::move(update_functions_.front());
        update_functions_.pop_front();
        update_func(scene_backend_.get());
    }

    // Trigger another update if more updates are needed still
    if (!update_functions_.empty()) {
        this->reset_draw_counter();
    }
}

void DisplayWindow::render(CameraPackage const& camera_package) const {
    scene_backend_->render(camera_package);
}

void DisplayWindow::configure_gui() {
    auto height = static_cast<float>(this->windowSize().y());
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, height), ImVec2(std::numeric_limits<float>::infinity(), height));
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    display_device_info();

    settings_.configure_gui();

    gvs::add_three_line_separator();

    gvs::configure_gui(&parent_scene_);

    ImGui::End();

    error_alert_->display_next_error();
}

void DisplayWindow::resize(const Vector2i& viewport) {
    scene_backend_->resize(viewport);
}

} // namespace ltb::gvs
