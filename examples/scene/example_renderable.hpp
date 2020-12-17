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
#include "grid_shader.hpp"
#include "ltb/gvs/display/backends/opengl_renderable.hpp"
#include "ltb/util/duration.hpp"

// external
#include <Magnum/GL/Mesh.h>

// standard
#include <memory>

namespace ltb::example {

class ExampleRenderable : public gvs::OpenglRenderable {
public:
    ExampleRenderable();

    // gvs::OpenGLRenderable start
    ~ExampleRenderable() override;

    auto configure_gui(gvs::DisplayInfo* display_info) -> bool override;

    auto init_gl_types(Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>& object,
                       Magnum::SceneGraph::DrawableGroup3D*                                    group,
                       unsigned intersect_id) -> void override;

    [[nodiscard]] auto drawable() const -> Magnum::SceneGraph::Drawable3D* override;

    auto set_display_info(gvs::DisplayInfo const& display_info) -> void override;

    auto resize(Magnum::Vector2i const& viewport) -> void override;
    // gvs::OpenGLRenderable end

    auto update(util::Duration const& sim_time) -> void;

private:
    class ExampleDrawable;

    std::shared_ptr<ExampleDrawable> drawable_;
};

} // namespace ltb::example
