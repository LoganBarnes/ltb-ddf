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
#include "box_shader.hpp"

// external
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

namespace ltb::gvs {

BoxShader::BoxShader() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL450);

    const Corrade::Utility::Resource rs{"gvs-resource-data"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader geom{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Geometry};
    Magnum::GL::Shader frag{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.get("box_shader.vert"));
    geom.addSource(rs.get("box_shader.geom"));
    frag.addSource(rs.get("general_shader.frag"));

    auto vert_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(vert);
    auto geom_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(geom);
    auto frag_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert_ref, geom_ref, frag_ref}));

    attachShaders({vert, geom, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    projection_from_world_uniform_location_ = uniformLocation("projection_from_world");

    coloring_uniform_location_      = uniformLocation("coloring");
    uniform_color_uniform_location_ = uniformLocation("uniform_color");

    shading_uniform_location_         = uniformLocation("shading");
    light_direction_uniform_location_ = uniformLocation("light_direction");
    light_color_uniform_location_     = uniformLocation("light_color");
    ambient_scale_uniform_location_   = uniformLocation("ambient_scale");
}

auto BoxShader::set_projection_from_world_matrix(Magnum::Matrix4 const& projection_from_world) -> BoxShader& {
    setUniform(projection_from_world_uniform_location_, projection_from_world);
    return *this;
}

auto BoxShader::set_coloring(Coloring const& coloring) -> BoxShader& {
    setUniform(coloring_uniform_location_, std::underlying_type_t<Coloring>(coloring));
    return *this;
}

auto BoxShader::set_uniform_color(Magnum::Color3 const& color) -> BoxShader& {
    setUniform(uniform_color_uniform_location_, color);
    return *this;
}

auto BoxShader::set_shading(Shading const& shading) -> BoxShader& {
    setUniform(shading_uniform_location_, std::underlying_type_t<Shading>(shading));
    return *this;
}

} // namespace ltb::gvs
