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
#include "make_primitive.hpp"

// project
#include "magnum_conversions.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Primitives/Cone.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Trade/MeshData.h>

namespace ltb::gvs {
namespace {

auto extract_positions_normal_indices(const Magnum::Trade::MeshData& mesh,
                                      AttributeVector<3>*            positions_out,
                                      AttributeVector<3>*            normals_out,
                                      std::vector<unsigned>*         indices_out) {
    if (positions_out) {
        auto positions = std::vector<Magnum::Vector3>(mesh.vertexCount());
        mesh.positions3DInto(positions);
        *positions_out = std::move(positions);
    }

    if (normals_out) {
        auto normals = std::vector<Magnum::Vector3>(mesh.vertexCount());
        mesh.normalsInto(normals);
        *normals_out = std::move(normals);
    }

    if (indices_out) {
        indices_out->resize(mesh.indexCount());
        mesh.indicesInto(*indices_out);
    }
}

struct PrimitiveVisitor {
    SceneItemInfo* info;

    auto operator()(Axes const&) const -> void {
        auto mesh = Magnum::Primitives::axis3D();

        auto colors4 = mesh.colorsAsArray();
        auto colors  = std::vector<Magnum::Color3>{colors4.size()};
        std::transform(colors4.begin(), colors4.end(), colors.begin(), [](const auto& color) {
            return Magnum::Color3{color.r(), color.g(), color.b()};
        });

        info->geometry_info = {};
        extract_positions_normal_indices(mesh, &info->geometry_info.positions, nullptr, &info->geometry_info.indices);
        info->geometry_info.vertex_colors  = std::move(colors);
        info->display_info.geometry_format = from_magnum(mesh.primitive());
        info->display_info.coloring        = gvs::Coloring::VertexColors;
        info->display_info.shading         = gvs::Shading::UniformColor;
    }

    auto operator()(Cone const& cone) const -> void {
        auto mesh = Magnum::Primitives::coneSolid(cone.rings, cone.segments, cone.half_length);

        info->geometry_info = {};
        extract_positions_normal_indices(mesh,
                                         &info->geometry_info.positions,
                                         &info->geometry_info.normals,
                                         &info->geometry_info.indices);
        info->display_info.geometry_format = from_magnum(mesh.primitive());
    }

    auto operator()(Cube const&) const -> void {
        auto mesh = Magnum::Primitives::cubeSolid();

        info->geometry_info = {};
        extract_positions_normal_indices(mesh,
                                         &info->geometry_info.positions,
                                         &info->geometry_info.normals,
                                         &info->geometry_info.indices);
        info->display_info.geometry_format = from_magnum(mesh.primitive());
    }

    auto operator()(Cylinder const& cylinder) const -> void {
        auto mesh = Magnum::Primitives::cylinderSolid(cylinder.rings, cylinder.segments, cylinder.half_length);

        info->geometry_info = {};
        extract_positions_normal_indices(mesh,
                                         &info->geometry_info.positions,
                                         &info->geometry_info.normals,
                                         &info->geometry_info.indices);
        info->display_info.geometry_format = from_magnum(mesh.primitive());
    }

    auto operator()(Plane const&) const -> void {
        auto mesh = Magnum::Primitives::planeSolid();

        info->geometry_info = {};
        extract_positions_normal_indices(mesh, &info->geometry_info.positions, &info->geometry_info.normals, nullptr);
        info->display_info.geometry_format = from_magnum(mesh.primitive());
    }

    auto operator()(Sphere const& sphere) const -> void {
        auto mesh = Magnum::Primitives::uvSphereSolid(sphere.rings, sphere.segments);

        info->geometry_info = {};
        extract_positions_normal_indices(mesh,
                                         &info->geometry_info.positions,
                                         &info->geometry_info.normals,
                                         &info->geometry_info.indices);
        info->display_info.geometry_format = from_magnum(mesh.primitive());
    }
};

} // namespace

auto make_primitive(SceneItemInfo* info, Primitive const& primitive) -> void {
    return mapbox::util::apply_visitor(PrimitiveVisitor{info}, primitive);
}

} // namespace ltb::gvs
