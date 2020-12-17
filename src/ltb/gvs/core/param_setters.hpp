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

#include "types.hpp"

// standard
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace ltb {
namespace gvs {
namespace detail {

/*
 * The following two classes are intended to only be used as rvalue references inside the
 * 'StaticScene::add_item', 'StaticScene::update_item', and 'StaticScene::get_item_info` functions.
 * All the move and copy constructors are deleted so the only way to use these classes is by
 * creating an instance as part of the function call:
 *
 * scene.add_item(gvs::SetPositions(positions), gvs::SetReadableID("Sphere"));
 *
 * where gvs::SetPositions and gvs::SetReadableID are the "named parameter" classes.
 *
 * The SceneSetter and SceneGetter classes are kept generic since they do the same thing for
 * many different types. To create a new named parameter we add a new class that derives from
 * SceneSetter or SceneGetter.
 *
 * The Curiously Recurring Template Pattern (CRTP) is used to avoid any indirection performance
 * costs typically associated with base classes. Because of this, each derived class will have
 * to implement a 'move_data' or 'update_data' function for the SceneSetter and SceneGetter classes
 * respectively.
 */

///
/// \brief A "named parameter" wrapper used to set items in a StaticScene
///
template <typename T, std::unique_ptr<T> SparseSceneItemInfo::*member>
struct SceneSetter {
    explicit SceneSetter(T value = {}) : data_(std::move(value)) {}

    auto operator()(SparseSceneItemInfo* info) -> std::string {
        if (info->*member) {
            return "Scene parameter already set.";
        }
        info->*member = std::make_unique<T>(std::move(data_));
        return "";
    }

    SceneSetter(const SceneSetter&)     = delete;
    SceneSetter(SceneSetter&&) noexcept = delete;
    SceneSetter& operator=(const SceneSetter&) = delete;
    SceneSetter& operator=(SceneSetter&&) noexcept = delete;

private:
    T data_;
};

///
/// \brief A "named parameter" wrapper used to set geometry info for items in a StaticScene
///
template <typename T, std::unique_ptr<T> SparseGeometryInfo::*member>
struct SceneGeometrySetter {
    explicit SceneGeometrySetter(T value = {}) : data_(std::move(value)) {}

    auto operator()(SparseSceneItemInfo* info) -> std::string {
        if (!info->geometry) {
            info->geometry = std::make_unique<Geometry>(SparseGeometryInfo{});
        } else if (info->geometry->is<Primitive>()) {
            return "Scene Geometry already specified as a primitive";
        }

        auto& geometry_info = info->geometry->get<SparseGeometryInfo>();

        if (geometry_info.*member) {
            return "Scene parameter already set.";
        }

        geometry_info.*member = std::make_unique<T>(std::move(data_));
        return "";
    }

    SceneGeometrySetter(const SceneGeometrySetter&)     = delete;
    SceneGeometrySetter(SceneGeometrySetter&&) noexcept = delete;
    SceneGeometrySetter& operator=(const SceneGeometrySetter&) = delete;
    SceneGeometrySetter& operator=(SceneGeometrySetter&&) noexcept = delete;

private:
    T data_;
};

///
/// \brief A "named parameter" wrapper used to set geometry indices for items in a StaticScene
///
template <GeometryFormat Format>
struct SceneIndicesSetter {
    explicit SceneIndicesSetter(std::vector<unsigned> value = {}) : data_(std::move(value)) {}

    auto operator()(SparseSceneItemInfo* info) -> std::string {
        if (!info->display_info) {
            info->display_info = std::make_unique<SparseDisplayInfo>();
        }
        if (!info->geometry) {
            info->geometry = std::make_unique<Geometry>(SparseGeometryInfo{});
        } else if (info->geometry->is<Primitive>()) {
            return "Scene Geometry already specified as a primitive";
        }

        auto& display_info  = *info->display_info;
        auto& geometry_info = info->geometry->get<SparseGeometryInfo>();

        if (display_info.geometry_format) {
            return "geometry_format already set.";
        }

        if (geometry_info.indices) {
            return "indices already set.";
        }

        geometry_info.indices        = std::make_unique<std::vector<unsigned>>(std::move(data_));
        display_info.geometry_format = std::make_unique<GeometryFormat>(Format);

        return "";
    }

    SceneIndicesSetter(const SceneIndicesSetter&)     = delete;
    SceneIndicesSetter(SceneIndicesSetter&&) noexcept = delete;
    SceneIndicesSetter& operator=(const SceneIndicesSetter&) = delete;
    SceneIndicesSetter& operator=(SceneIndicesSetter&&) noexcept = delete;

private:
    std::vector<unsigned> data_;
};

///
/// \brief A "named parameter" wrapper used to set display info for items in a StaticScene
///
template <typename T, std::unique_ptr<T> SparseDisplayInfo::*member>
struct SceneDisplaySetter {
    explicit SceneDisplaySetter(T value = {}) : data(std::move(value)) {}

    auto operator()(SparseSceneItemInfo* info) -> std::string {
        if (!info->display_info) {
            info->display_info = std::make_unique<SparseDisplayInfo>();
        }

        auto& display_info = *info->display_info;

        if (display_info.*member) {
            return "Scene parameter already set.";
        }

        display_info.*member = std::make_unique<T>(std::move(data));
        return "";
    }

    SceneDisplaySetter(const SceneDisplaySetter&)     = delete;
    SceneDisplaySetter(SceneDisplaySetter&&) noexcept = delete;
    SceneDisplaySetter& operator=(const SceneDisplaySetter&) = delete;
    SceneDisplaySetter& operator=(SceneDisplaySetter&&) noexcept = delete;

protected:
    T data;
};

} // namespace detail

///
/// \brief A "named parameter" wrapper used to set geometry info for items in a StaticScene
///
struct SetPrimitive {
    explicit SetPrimitive(Primitive value) : data_(std::move(value)) {}

    auto operator()(SparseSceneItemInfo* info) -> std::string {
        if (info->geometry) {
            return "Geometry already specified";
        }

        info->geometry = std::make_unique<Geometry>(std::move(data_));
        return "";
    }

    SetPrimitive(const SetPrimitive&)     = delete;
    SetPrimitive(SetPrimitive&&) noexcept = delete;
    SetPrimitive& operator=(const SetPrimitive&) = delete;
    SetPrimitive& operator=(SetPrimitive&&) noexcept = delete;

private:
    Primitive data_;
};

/*
 * Setters
 */
using SetPositions3d = detail::SceneGeometrySetter<AttributeVector<3>, &SparseGeometryInfo::positions>;
using SetNormals3d   = detail::SceneGeometrySetter<AttributeVector<3>, &SparseGeometryInfo::normals>;
using SetTextureCoordinates3d
    = detail::SceneGeometrySetter<AttributeVector<2>, &SparseGeometryInfo::texture_coordinates>;
using SetVertexColors3d = detail::SceneGeometrySetter<AttributeVector<3>, &SparseGeometryInfo::vertex_colors>;
using SetIndices        = detail::SceneGeometrySetter<std::vector<unsigned>, &SparseGeometryInfo::indices>;

using SetPoints        = detail::SceneIndicesSetter<GeometryFormat::Points>;
using SetLines         = detail::SceneIndicesSetter<GeometryFormat::Lines>;
using SetLineStrip     = detail::SceneIndicesSetter<GeometryFormat::LineStrip>;
using SetTriangles     = detail::SceneIndicesSetter<GeometryFormat::Triangles>;
using SetTriangleStrip = detail::SceneIndicesSetter<GeometryFormat::TriangleStrip>;
using SetTriangleFan   = detail::SceneIndicesSetter<GeometryFormat::TriangleFan>;

using SetParent = detail::SceneSetter<SceneId, &SparseSceneItemInfo::parent>;

using SetReadableId     = detail::SceneDisplaySetter<std::string, &SparseDisplayInfo::readable_id>;
using SetGeometryFormat = detail::SceneDisplaySetter<GeometryFormat, &SparseDisplayInfo::geometry_format>;
using SetTransformation = detail::SceneDisplaySetter<mat4, &SparseDisplayInfo::transformation>;
using SetUniformColor   = detail::SceneDisplaySetter<vec3, &SparseDisplayInfo::uniform_color>;
using SetColoring       = detail::SceneDisplaySetter<Coloring, &SparseDisplayInfo::coloring>;
using SetShading        = detail::SceneDisplaySetter<Shading, &SparseDisplayInfo::shading>;
using SetVisible        = detail::SceneDisplaySetter<bool, &SparseDisplayInfo::visible>;
using SetOpacity        = detail::SceneDisplaySetter<float, &SparseDisplayInfo::opacity>;
using SetWireframeOnly  = detail::SceneDisplaySetter<bool, &SparseDisplayInfo::wireframe_only>;

} // namespace gvs
} // namespace ltb
