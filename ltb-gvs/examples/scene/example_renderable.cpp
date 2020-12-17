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
#include "example_renderable.hpp"

// project
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/gvs/display/magnum_conversions.hpp"

// external
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <imgui.h>

using namespace Magnum;

namespace ltb::example {

class ExampleRenderable::ExampleDrawable : public SceneGraph::Drawable3D {
public:
    explicit ExampleDrawable(SceneGraph::Object<SceneGraph::MatrixTransformation3D>& object,
                             SceneGraph::DrawableGroup3D*                            group,
                             unsigned                                                intersect_id)
        : SceneGraph::Drawable3D{object, group}, object_(object), intersect_id_(intersect_id) {
        mesh_.setPrimitive(MeshPrimitive::Points);
        set_grid_divisions(50);
    }

    ~ExampleDrawable() override = default;

    auto update_display_info(const gvs::DisplayInfo& display_info) -> void {
        object_.setTransformation(gvs::to_magnum(display_info.transformation));
        color_ = gvs::to_magnum<Magnum::Color3>(display_info.uniform_color);
    }

    auto set_grid_divisions(int grid_divisions) -> void {
        grid_divisions_ = grid_divisions;
        mesh_.setCount((grid_divisions_ + 2) * (grid_divisions_ + 2));
    }

    auto grid_divisions() const -> int { return grid_divisions_; }

    auto update_time(float seconds) -> void { sim_time_secs_ = seconds; }

    float grid_width = 10.f;

private:
    void draw(Matrix4 const& transformation_matrix, SceneGraph::Camera3D& camera) override {
        shader_.set_projection_from_world_matrix(camera.projectionMatrix() * transformation_matrix)
            .set_color(color_)
            .set_grid_width(grid_width)
            .set_grid_divisions(grid_divisions_)
            .set_id(intersect_id_)
            .set_time(sim_time_secs_)
            .draw(mesh_);
    }

    SceneGraph::Object<SceneGraph::MatrixTransformation3D>& object_;

    Color3 color_ = {gvs::default_uniform_color[0], gvs::default_uniform_color[1], gvs::default_uniform_color[2]};
    int    grid_divisions_ = 0;

    unsigned intersect_id_  = 0u;
    float    sim_time_secs_ = 0.f;

    Magnum::GL::Mesh mesh_;
    GridShader       shader_;
};

ExampleRenderable::ExampleRenderable() = default;

ExampleRenderable::~ExampleRenderable() = default;

auto ExampleRenderable::configure_gui(gvs::DisplayInfo* display_info) -> bool {
    auto something_changed = false;

    if (drawable_) {
        if (ImGui::DragFloat("Grid Width", &drawable_->grid_width, 0.1f, 1.f, 15.f)) {
            drawable_->grid_width = std::min(std::max(drawable_->grid_width, 1.f), 15.f);
            something_changed     = true;
        }

        auto grid_divisions = drawable_->grid_divisions();
        if (ImGui::DragInt("Grid Divisions", &grid_divisions, 1.f, 0, 100)) {
            grid_divisions = std::min(std::max(grid_divisions, 0), 100);
            drawable_->set_grid_divisions(grid_divisions);
            something_changed = true;
        }
    }

    something_changed |= (display_info && ImGui::ColorEdit3("Color", display_info->uniform_color.data()));

    return something_changed;
}

auto ExampleRenderable::init_gl_types(SceneGraph::Object<SceneGraph::MatrixTransformation3D>& object,
                                      SceneGraph::DrawableGroup3D*                            group,
                                      unsigned                                                intersect_id) -> void {
    drawable_ = std::make_shared<ExampleDrawable>(object, group, intersect_id);
}

auto ExampleRenderable::drawable() const -> SceneGraph::Drawable3D* {
    return drawable_.get();
}
auto ExampleRenderable::set_display_info(gvs::DisplayInfo const& display_info) -> void {
    drawable_->update_display_info(display_info);
}

auto ExampleRenderable::resize(Vector2i const & /*viewport*/) -> void {}

auto ExampleRenderable::update(util::Duration const& sim_time) -> void {
    drawable_->update_time(util::to_seconds<float>(sim_time));
}

} // namespace ltb::example
