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
#include "main_window.hpp"

// project
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/gvs/display/magnum_conversions.hpp"

// external
#include <imgui.h>

using namespace Magnum;

namespace ltb::example {
namespace {

constexpr gvs::vec3 blueish  = {0.3f, 0.3f, 1.f};
constexpr gvs::vec3 greenish = {0.3f, 1.f, 0.3f};
constexpr gvs::vec3 redish   = {1.f, 0.3f, 0.3f};
constexpr gvs::vec3 orange   = {1.f, 0.5f, 0.1f};

} // namespace

MainWindow::MainWindow(const Arguments& arguments)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Intersection Example")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)) {

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));

    shapes_root_ = scene_.add_item(gvs::SetPositions3d(),
                                   gvs::SetTransformation(shapes_transform_),
                                   gvs::SetReadableId("Shapes"));

    gvs::SceneId cube;
    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +4.0f; // x
        translation[13]  = +0.0f; // y
        translation[14]  = +0.0f; // z

        cube = scene_.add_item(gvs::SetReadableId("Cube"),
                               gvs::SetPrimitive(gvs::Cube{}),
                               gvs::SetTransformation(translation),
                               gvs::SetUniformColor(blueish),
                               gvs::SetColoring(gvs::Coloring::UniformColor),
                               gvs::SetShading(gvs::Shading::Lambertian),
                               gvs::SetParent(shapes_root_));
    }

    gvs::SceneId sphere;
    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +2.0f; // x
        translation[13]  = +2.0f; // y
        translation[14]  = -2.0f; // z

        sphere = scene_.add_item(gvs::SetReadableId("Sphere"),
                                 gvs::SetPrimitive(gvs::Sphere{}),
                                 gvs::SetTransformation(translation),
                                 gvs::SetUniformColor(greenish),
                                 gvs::SetColoring(gvs::Coloring::UniformColor),
                                 gvs::SetShading(gvs::Shading::Lambertian),
                                 gvs::SetParent(cube));
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = -3.0f; // x
        translation[13]  = -5.0f; // y
        translation[14]  = +0.0f; // z

        scene_.add_item(gvs::SetReadableId("Cylinder"),
                        gvs::SetPrimitive(gvs::Cylinder{}),
                        gvs::SetTransformation(translation),
                        gvs::SetUniformColor(redish),
                        gvs::SetColoring(gvs::Coloring::UniformColor),
                        gvs::SetShading(gvs::Shading::Lambertian),
                        gvs::SetParent(sphere));
    }

    gvs::SceneId cone;
    {
        auto translation = gvs::identity_mat4;
        translation[12]  = -2.0f; // x
        translation[13]  = +1.0f; // y
        translation[14]  = +0.0f; // z

        cone = scene_.add_item(gvs::SetReadableId("Cone"),
                               gvs::SetPrimitive(gvs::Cone{}),
                               gvs::SetTransformation(translation),
                               gvs::SetColoring(gvs::Coloring::UniformColor),
                               gvs::SetShading(gvs::Shading::Lambertian),
                               gvs::SetParent(shapes_root_));
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +0.0f; // x
        translation[13]  = +2.0f; // y
        translation[14]  = +0.0f; // z

        scene_.add_item(gvs::SetReadableId("Plane"),
                        gvs::SetPrimitive(gvs::Plane{}),
                        gvs::SetTransformation(translation),
                        gvs::SetUniformColor(orange),
                        gvs::SetColoring(gvs::Coloring::UniformColor),
                        gvs::SetShading(gvs::Shading::Lambertian),
                        gvs::SetParent(cone));
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {
    // Nothing to do
}

void MainWindow::render(const gvs::CameraPackage& camera_package) const {
    scene_.render(camera_package);
}

void MainWindow::configure_gui() {
    auto height = static_cast<float>(this->windowSize().y());
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints({0.f, height}, {std::numeric_limits<float>::infinity(), height});
    ImGui::SetNextWindowSize({350.f, height}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings", nullptr);

    gvs::display_device_info();

    gvs::add_three_line_separator();

    if (ImGui::DragFloat3("Global Translation", &shapes_transform_[12], 0.1f)) {
        scene_.update_item(shapes_root_, gvs::SetTransformation(shapes_transform_));
    }

    gvs::configure_gui(&scene_);

    ImGui::End();

    error_alert_->display_next_error();
}

void MainWindow::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

} // namespace ltb::example

auto main(int argc, char* argv[]) -> int {
    ltb::example::MainWindow app({argc, argv});
    return app.exec();
}
