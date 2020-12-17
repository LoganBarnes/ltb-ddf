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

#include "forward_declarations.hpp"
#include "gui/imgui_magnum_application.hpp"
#include "ltb/util/blocking_queue.hpp"
#include "scene_update_func.hpp"

// standard
#include <thread>

namespace ltb::gvs {

class DisplayWindow : public ImGuiMagnumApplication {
public:
    explicit DisplayWindow(DisplayScene& parent_scene);
    ~DisplayWindow() override;

    auto thread_safe_update(SceneUpdateFunc update_func) -> void;

private:
    void update() override;
    void render(CameraPackage const& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    // Scene
    DisplayScene&                   parent_scene_;
    std::unique_ptr<DisplayBackend> scene_backend_; ///< Used to update and display the scene

    // Data processing
    std::mutex                  update_lock_;
    std::deque<SceneUpdateFunc> update_functions_;
};

} // namespace ltb::gvs
