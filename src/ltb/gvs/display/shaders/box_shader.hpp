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
#include "ltb/gvs/core/types.hpp"

// external
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>

namespace ltb {
namespace gvs {

class BoxShader : public Magnum::GL::AbstractShaderProgram {
public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector4> CenterAndRadius;

    explicit BoxShader();

    auto set_projection_from_world_matrix(Magnum::Matrix4 const& projection_from_world) -> BoxShader&;

    auto set_coloring(Coloring const& coloring) -> BoxShader&;
    auto set_uniform_color(Magnum::Color3 const& color) -> BoxShader&;

    auto set_shading(Shading const& shading) -> BoxShader&;

private:
    int projection_from_world_uniform_location_ = -1;

    int coloring_uniform_location_      = -1;
    int uniform_color_uniform_location_ = -1;

    int shading_uniform_location_         = -1;
    int light_direction_uniform_location_ = -1;
    int light_color_uniform_location_     = -1;
    int ambient_scale_uniform_location_   = -1;
};

} // namespace gvs
} // namespace ltb
