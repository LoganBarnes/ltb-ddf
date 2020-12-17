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
#include "ltb/gvs/display/gui/error_alert.hpp"
#include "ltb/gvs/display/gui/imgui_magnum_application.hpp"
#include "ltb/gvs/display/local_scene.hpp"

namespace ltb::example {

class MainWindow : public gvs::ImGuiMagnumApplication {
public:
    explicit MainWindow(const Arguments& arguments);
    ~MainWindow() override;

private:
    void update() override;
    void render(const gvs::CameraPackage& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    // Axes
    struct {
        gvs::SceneId scene_id = gvs::nil_id();
    } axes_;

    // Cone
    struct {
        gvs::SceneId scene_id    = gvs::nil_id();
        unsigned     rings       = 35u;
        unsigned     segments    = 35u;
        float        half_length = 1.f;
    } cone_;

    // Cube
    struct {
        gvs::SceneId scene_id = gvs::nil_id();
    } cube_;

    // Cylinder
    struct {
        gvs::SceneId scene_id    = gvs::nil_id();
        unsigned     rings       = 35u;
        unsigned     segments    = 35u;
        float        half_length = 1.f;
    } cylinder_;

    // Plane
    struct {
        gvs::SceneId scene_id = gvs::nil_id();
    } plane_;

    // Sphere
    struct {
        gvs::SceneId scene_id = gvs::nil_id();
        unsigned     rings    = 35u;
        unsigned     segments = 35u;
    } sphere_;

    // Scene
    gvs::LocalScene scene_;

    auto reset_axes() -> void;
    auto reset_cone() -> void;
    auto reset_cube() -> void;
    auto reset_cylinder() -> void;
    auto reset_plane() -> void;
    auto reset_sphere() -> void;
};

} // namespace ltb::example
