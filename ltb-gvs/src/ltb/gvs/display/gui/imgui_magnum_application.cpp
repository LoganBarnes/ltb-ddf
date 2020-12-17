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
#include "imgui_magnum_application.hpp"

// project
#include "imgui_theme.hpp"

// external
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <imgui.h>

// standard
#include <algorithm>

static void initialize_resources() {
    CORRADE_RESOURCE_INITIALIZE(ltb_gvs_display_RESOURCES)
}

namespace ltb::gvs {

using namespace Magnum;
using namespace Math::Literals;

namespace {

constexpr auto camera_near_dist = 1.f;
constexpr auto camera_far_dist  = 1000.f;
constexpr auto camera_fovy_deg  = 45.0;

} // namespace

ImGuiMagnumApplication::ImGuiMagnumApplication(const Arguments& arguments, const Configuration& configuration)
    : GlfwApplication(arguments, configuration, GLConfiguration().setSampleCount(4)),
      imgui_(Vector2{windowSize()} / dpiScaling(), windowSize(), framebufferSize()),
      error_alert_(std::make_shared<ErrorAlert>("Error Popup")) {

    ImGui::GetCurrentContext()->SettingsHandlers.push_back(settings_.handler());

    initialize_resources();

    this->startTextInput(); // allow for text input callbacks

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();

    // Some fonts are loaded in the theme class but we might also want the default so it's loaded here
    theme_ = std::make_unique<GuiTheme>();

    GuiTheme::set_style();
    GL::Renderer::setClearColor({theme_->background.Value.x,
                                 theme_->background.Value.y,
                                 theme_->background.Value.z,
                                 theme_->background.Value.w});

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    /*
     * Camera
     */
    const Vector3 eye = Vector3::zAxis(25.0f);
    const Vector3 center{};
    const Vector3 up = Vector3::yAxis();
    arcball_camera_.emplace(camera_scene_,
                            eye,
                            center,
                            up,
                            Math::Deg<float>(camera_fovy_deg),
                            camera_near_dist,
                            camera_far_dist,
                            windowSize(),
                            framebufferSize());

    camera_package_.camera = arcball_camera_->camera();

    reset_draw_counter();
}

ImGuiMagnumApplication::ImGuiMagnumApplication(const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("ImGui Magnum Application")
                                 .setWindowFlags(Configuration::WindowFlag::Resizable)) {}

ImGuiMagnumApplication::~ImGuiMagnumApplication() = default;

auto ImGuiMagnumApplication::reset_draw_counter() -> void {
    draw_counter_ = 5;
    redraw();
    glfwPostEmptyEvent();
}

auto ImGuiMagnumApplication::drawEvent() -> void {
    update();

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    auto cam_changed               = arcball_camera_->update();
    camera_package_.focal_distance = arcball_camera_->viewDistance();

    render(camera_package_);

    imgui_.newFrame();

    configure_gui();

    /* Set appropriate states. If you only draw imgui UI, it is sufficient to
       do this once in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

    imgui_.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state next frame. */
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();

    if (cam_changed) {
        reset_draw_counter();
    }

    if (draw_counter_-- > 0) {
        redraw();
    }
}

auto ImGuiMagnumApplication::viewportEvent(ViewportEvent& event) -> void {
    reset_draw_counter();
    GL::defaultFramebuffer.setViewport({{}, event.windowSize()});

    imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());

    //    camera_package_.update_viewport(event.windowSize());
    arcball_camera_->reshape(event.windowSize(), event.framebufferSize());

    resize(event.windowSize());
}

auto ImGuiMagnumApplication::keyPressEvent(KeyEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleKeyPressEvent(event)) {
        return;
    }

    handleKeyPressEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);
}

auto ImGuiMagnumApplication::keyReleaseEvent(KeyEvent& event) -> void {
    reset_draw_counter();

    using KE = KeyEvent;

    const auto ctrl_down  = static_cast<bool>(event.modifiers() & KE::Modifier::Ctrl);
    const auto shift_down = static_cast<bool>(event.modifiers() & KE::Modifier::Shift);

    // Ctrl + Shift + Q == Exit
    if (ctrl_down and shift_down and event.key() == KE::Key::Q) {

        this->exit();
        return;
    }

    if (imgui_.handleKeyReleaseEvent(event)) {
        return;
    }

    handleKeyReleaseEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);

    switch (event.key()) {

    case KE::Key::L:
        if (arcball_camera_->lagging() > 0.0f) {
            Debug{} << "Lagging disabled";
            arcball_camera_->setLagging(0.0f);
        } else {
            Debug{} << "Lagging enabled";
            arcball_camera_->setLagging(0.85f);
        }
        break;

    case KE::Key::R: {
        if (ctrl_down) {
            arcball_camera_->reset();
        } else {
            arcball_camera_->resetViewCenter(Vector3{});
            arcball_camera_->update(true);
        }
    } break;

    default:
        break;
    }
}

auto ImGuiMagnumApplication::textInputEvent(TextInputEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleTextInputEvent(event)) {
        return;
    }

    handleTextInputEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);
}

auto ImGuiMagnumApplication::mousePressEvent(MouseEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMousePressEvent(event)) {
        return;
    }

    handleMousePressEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);

    arcball_camera_->initTransformation(event.position());
    reset_draw_counter();
}

auto ImGuiMagnumApplication::mouseReleaseEvent(MouseEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMouseReleaseEvent(event)) {
        return;
    }

    handleMouseReleaseEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);
}

auto ImGuiMagnumApplication::mouseMoveEvent(MouseMoveEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMouseMoveEvent(event)) {
        return;
    }

    handleMouseMoveEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);

    // make these editable settings
    auto orbiting = (event.buttons() & settings_.camera.orbit_button);
    auto panning  = (event.buttons() & settings_.camera.pan_button);

    if (panning) {
        arcball_camera_->translate(event.position());
    } else if (orbiting) {
        arcball_camera_->rotate(event.position());
    }
    reset_draw_counter();
}

auto ImGuiMagnumApplication::mouseScrollEvent(MouseScrollEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMouseScrollEvent(event)) {
        return;
    }

    handleMouseScrollEvent(event);
    if (event.isAccepted()) {
        return;
    }

    event.setAccepted(true);

    if (event.offset().y() == 0.f) {
        return;
    }

    constexpr auto wheel_zoom_scale = 1.f; // make this an editable setting

    const auto delta = event.offset().y();
    if (std::abs(delta) >= 1.0e-2f) {
        arcball_camera_->zoom(delta * wheel_zoom_scale);
        reset_draw_counter();
    }
}

} // namespace ltb::gvs
