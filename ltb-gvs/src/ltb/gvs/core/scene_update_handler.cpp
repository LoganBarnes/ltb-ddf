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
#include "scene_update_handler.hpp"

namespace ltb {
namespace gvs {

UpdatedInfo::UpdatedInfo(SparseSceneItemInfo const& info) {
    if (info.geometry) {
        auto const& geom = *info.geometry;

        geometry |= true;

        if (geom.is<Primitive>()) {
            geometry_vertices |= true;
            geometry_indices |= true;
            display |= true;
            display_geometry_format |= true;
        } else {
            assert(geom.is<SparseGeometryInfo>());
            auto const& geometry_info = geom.get<SparseGeometryInfo>();

            geometry_vertices |= (geometry_info.positions || geometry_info.normals || geometry_info.texture_coordinates
                                  || geometry_info.vertex_colors);
            geometry_indices |= (!!geometry_info.indices);
        }
    }

    if (info.display_info) {
        display |= true;
        display_geometry_format |= (!!info.display_info->geometry_format);
        display_visible |= (!!info.display_info->visible);
    }

    parent   = (!!info.parent);
    children = (!!info.children);
}

auto UpdatedInfo::everything() -> UpdatedInfo {
    UpdatedInfo info;
    info.geometry                = true;
    info.geometry_vertices       = true;
    info.geometry_indices        = true;
    info.display                 = true;
    info.display_geometry_format = true;
    info.display_visible         = true;
    info.parent                  = true;
    info.children                = true;
    return info;
}

auto UpdatedInfo::children_only() -> UpdatedInfo {
    UpdatedInfo info = {};
    info.children    = true;
    return info;
}

SceneUpdateHandler::~SceneUpdateHandler() = default;

} // namespace gvs
} // namespace ltb
