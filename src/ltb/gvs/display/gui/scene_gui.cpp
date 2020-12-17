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
#include "scene_gui.hpp"

// project
#include "imgui_utils.hpp"
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/gvs/core/scene.hpp"

// external
#include <imgui.h>

namespace ltb::gvs {
namespace {

auto configure_scene_gui(SceneId const& item_id, Scene* scene) -> bool {
    const ImVec4 gray = {0.5f, 0.5f, 0.5f, 1.f};

    DisplayInfo display_info;

    bool has_geometry{};
    bool has_children{};

    std::shared_ptr<CustomRenderable> renderable;

    scene->get_item_info(item_id,
                         GetRenderable(&renderable),
                         GetReadableId(&display_info.readable_id),
                         GetGeometryFormat(&display_info.geometry_format),
                         GetTransformation(&display_info.transformation),
                         GetUniformColor(&display_info.uniform_color),
                         GetColoring(&display_info.coloring),
                         GetShading(&display_info.shading),
                         IsVisible(&display_info.visible),
                         GetOpacity(&display_info.opacity),
                         IsWireframeOnly(&display_info.wireframe_only),
                         HasGeometry(&has_geometry),
                         HasChildren(&has_children));

    auto const id_str = to_string(item_id);
    ScopedID   scoped_id(id_str.c_str());

    bool item_changed     = false;
    bool children_changed = false;

    item_changed |= ImGui::Checkbox("###visible", &display_info.visible);
    ImGui::SameLine();

    auto tree_label = display_info.readable_id + "###" + id_str;

    if (ImGui::TreeNode(tree_label.c_str())) {
        ScopedIndent scoped_indent;
        ImGui::Separator();

        if (renderable) {
            item_changed |= renderable->configure_gui(&display_info);
        } else {
            item_changed |= configure_gui(&display_info, !has_geometry);
        }

        if (has_children) {
            ImGui::TextColored(gray, "Children:");

            std::vector<SceneId> children;
            scene->get_item_info(item_id, GetChildren(&children));

            for (auto const& child_id : children) {
                children_changed |= configure_scene_gui(child_id, scene);
            }

        } else {
            ImGui::TextColored(ImVec4(1.f, 1.f, 0.1f, 1.f), "No Children");
        }

        ImGui::TreePop();
    }

    if (item_changed) {
        scene->update_item(item_id,
                           SetReadableId(display_info.readable_id),
                           SetGeometryFormat(display_info.geometry_format),
                           SetTransformation(display_info.transformation),
                           SetUniformColor(display_info.uniform_color),
                           SetColoring(display_info.coloring),
                           SetShading(display_info.shading),
                           SetVisible(display_info.visible),
                           SetOpacity(display_info.opacity),
                           SetWireframeOnly(display_info.wireframe_only));
    }

    return item_changed | children_changed;
}

} // namespace

auto configure_gui(Coloring* coloring) -> bool {
    auto icoloring = std::underlying_type_t<Coloring>(*coloring);
    if (ImGui::Combo("Display Mode",
                     &icoloring,
                     " Positions \0"
                     " Normals \0"
                     " Texture Coordinates \0"
                     " Vertex Colors \0"
                     " Uniform Color \0"
                     " Texture \0"
                     " White \0"
                     "\0")) {
        *coloring = static_cast<Coloring>(icoloring);
        return true;
    }
    return false;
}

auto configure_gui(GeometryFormat* geometry_format) -> bool {
    auto igeometry_format = std::underlying_type_t<GeometryFormat>(*geometry_format);
    if (ImGui::Combo("Geometry Format",
                     &igeometry_format,
                     " Points \0"
                     " Lines \0"
                     " Line Strip \0"
                     " Triangles \0"
                     " Triangle Strip \0"
                     " Triangle Fan \0"
                     "\0")) {
        *geometry_format = static_cast<GeometryFormat>(igeometry_format);
        return true;
    }
    return false;
}

auto configure_gui(Shading* shading) -> bool {
    auto ishading = std::underlying_type_t<Shading>(*shading);
    if (ImGui::Combo("Shading",
                     &ishading,
                     " Uniform Color \0"
                     " Lambertian \0"
                     " Cook-Torrance \0"
                     "\0")) {
        *shading = static_cast<Shading>(ishading);
        return true;
    }
    return false;
}

auto configure_gui(DisplayInfo* display_info, bool display_name_only) -> bool {
    bool something_changed = false;
    something_changed |= configure_gui("###readable_id", &display_info->readable_id);

    if (display_info->visible && !display_name_only) {
        something_changed |= ImGui::Checkbox("Wireframe", &display_info->wireframe_only);

        something_changed |= configure_gui(&display_info->coloring);
        something_changed |= configure_gui(&display_info->geometry_format);
        something_changed |= configure_gui(&display_info->shading);

        if (display_info->coloring == Coloring::UniformColor) {
            something_changed |= ImGui::ColorEdit3("Global Color", display_info->uniform_color.data());
        }

        {
            Disable::Guard disable_opacity(true /*wireframe_only*/);
            something_changed |= ImGui::DragFloat("Opacity", &display_info->opacity, 0.01f, 0.f, 1.f);
        }
    }

    return something_changed;
}

auto configure_gui(Scene* scene) -> bool {
    std::vector<SceneId> children;
    scene->get_item_info(nil_id(), GetChildren(&children));

    ImGui::Text("Scene Items:");

    if (children.empty()) {
        ImGui::SameLine();
        ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, " (empty)");
        return false;
    }

    bool scene_changed = false;

    for (auto const& child_id : children) {
        scene_changed |= configure_scene_gui(child_id, scene);
    }

    return scene_changed;
}

} // namespace ltb::gvs
