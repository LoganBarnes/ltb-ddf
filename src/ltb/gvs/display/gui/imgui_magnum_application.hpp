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
#include "error_alert.hpp"
#include "imgui_theme.hpp"
#include "ltb/gvs/display/camera_package.hpp"
#include "ltb/gvs/forward_declarations.hpp"
#include "settings.hpp"

// external
#include <Magnum/ArcBallCamera.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

// system
#include <memory>
#include <optional>

namespace ltb::gvs {

class ImGuiMagnumApplication : public Magnum::Platform::Application {
public:
    explicit ImGuiMagnumApplication(const Arguments& arguments, const Configuration& configuration);
    explicit ImGuiMagnumApplication(const Arguments& arguments);
    virtual ~ImGuiMagnumApplication();

    // keyboard
    virtual auto handleKeyPressEvent(KeyEvent & /*event*/) -> void {}
    virtual auto handleKeyReleaseEvent(KeyEvent & /*event*/) -> void {}
    virtual auto handleTextInputEvent(TextInputEvent & /*event*/) -> void {}

    // mouse
    virtual auto handleMousePressEvent(MouseEvent & /*event*/) -> void {}
    virtual auto handleMouseReleaseEvent(MouseEvent & /*event*/) -> void {}
    virtual auto handleMouseMoveEvent(MouseMoveEvent & /*event*/) -> void {}
    virtual auto handleMouseScrollEvent(MouseScrollEvent & /*event*/) -> void {}

private:
    virtual auto update() -> void                                          = 0;
    virtual auto render(CameraPackage const& camera_package) const -> void = 0;
    virtual auto configure_gui() -> void                                   = 0;

    virtual auto resize(const Magnum::Vector2i& viewport) -> void = 0;

    auto drawEvent() -> void override;
    auto viewportEvent(ViewportEvent& event) -> void override;

    // keyboard
    auto keyPressEvent(KeyEvent& event) -> void override;
    auto keyReleaseEvent(KeyEvent& event) -> void override;
    auto textInputEvent(TextInputEvent& event) -> void override;

    // mouse
    auto mousePressEvent(MouseEvent& event) -> void override;
    auto mouseReleaseEvent(MouseEvent& event) -> void override;
    auto mouseMoveEvent(MouseMoveEvent& event) -> void override;
    auto mouseScrollEvent(MouseScrollEvent& event) -> void override;

    Magnum::ImGuiIntegration::Context imgui_{Magnum::NoCreate};

    // Camera
    Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D> camera_scene_;

    int draw_counter_ = 1; /// continue drawing until this counter is zero

protected:
    // forward declaration
    std::unique_ptr<GuiTheme> theme_;

    // General Info
    Settings settings_;

    // Camera
    std::optional<Magnum::ArcBallCamera> arcball_camera_;
    CameraPackage                        camera_package_;

    // Errors
    std::shared_ptr<ErrorAlert> error_alert_;

    // Ensures the application renders at least 5 more times after all events are
    // finished to give ImGui a chance to update and render correctly.
    auto reset_draw_counter() -> void;
};

} // namespace ltb::gvs
