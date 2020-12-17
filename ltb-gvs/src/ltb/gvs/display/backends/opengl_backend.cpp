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
#include "opengl_backend.hpp"

// project
#include "ltb/gvs/display/magnum_conversions.hpp"
#include "ltb/util/container_utils.hpp"
#include "ltb/util/result.hpp"
#include "ltb/util/variant_utils.hpp"
#include "opengl_renderable.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>

// standard
#include <iostream>

using namespace Magnum;

namespace ltb::gvs {
namespace {

auto update_vbo(MeshData* mesh_data, GeometryInfo const& geometry_info) -> void {

    std::vector<float> buffer_data;
    GLintptr           offset = 0;
    mesh_data->vbo_count      = 0;

    auto update_attribute = [&mesh_data, &offset, &buffer_data](auto const& attribute, auto const& shader_attribute) {
        buffer_data.insert(buffer_data.end(), attribute.data(), (attribute.data() + attribute.size()));
        mesh_data->mesh.addVertexBuffer(mesh_data->vertex_buffer, offset, shader_attribute);

        auto isize = static_cast<int>(attribute.size());
        offset += isize * static_cast<int>(sizeof(float));
        return isize;
    };

    if (!geometry_info.positions.empty()) {
        auto attribute_size  = update_attribute(geometry_info.positions, GeneralShader::Position{});
        mesh_data->vbo_count = attribute_size / 3;
    }

    if (!geometry_info.normals.empty()) {
        update_attribute(geometry_info.normals, GeneralShader::Normal{});
    }

    if (!geometry_info.texture_coordinates.empty()) {
        update_attribute(geometry_info.texture_coordinates, GeneralShader::TextureCoordinate{});
    }

    if (!geometry_info.vertex_colors.empty()) {
        update_attribute(geometry_info.vertex_colors, GeneralShader::VertexColor{});
    }

    mesh_data->vertex_buffer.setData(buffer_data, GL::BufferUsage::StaticDraw);
    mesh_data->mesh.setCount(mesh_data->vbo_count);
}

auto update_ibo(MeshData* mesh_data, std::vector<unsigned> const& indices) {
    if (!indices.empty()) {
        Corrade::Containers::Array<char> index_data;
        MeshIndexType                    index_type;
        std::tie(index_data, index_type) = MeshTools::compressIndices(indices);
        mesh_data->index_buffer.setData(index_data, GL::BufferUsage::StaticDraw);

        mesh_data->ibo_count = static_cast<int>(indices.size());

        mesh_data->mesh.setCount(mesh_data->ibo_count).setIndexBuffer(mesh_data->index_buffer, 0, index_type);
    }
}

} // namespace

OpenglBackend::OpenglItem::OpenglItem(
    SceneId id, Object3D* obj, SceneGraph::DrawableGroup3D* drawables, unsigned id_for_intersect, GeneralShader& shader)
    : scene_id(id), intersect_id(id_for_intersect), data(MeshData()), object(obj) {
    auto& mesh_data = std::get<MeshData>(data);
    mesh_data.mesh.setCount(0).setPrimitive(to_magnum(default_geometry_format));
    mesh_data.drawable = new OpaqueDrawable(*object, drawables, mesh_data.mesh, intersect_id, shader);
}

OpenglBackend::OpenglItem::OpenglItem(SceneId                      id,
                                      Object3D*                    obj,
                                      SceneGraph::DrawableGroup3D* drawables,
                                      unsigned                     id_for_intersect,
                                      OpenglRenderable*            ogl_renderable)
    : scene_id(id), intersect_id(id_for_intersect), data(ogl_renderable), object(obj) {
    auto* renderable = std::get<OpenglRenderable*>(data);
    renderable->init_gl_types(*object, drawables, id_for_intersect);
}

auto OpenglBackend::OpenglItem::drawable() const -> SceneGraph::Drawable3D* {
    return std::visit(util::Visitor{
                          [](const MeshData& mesh_data) -> SceneGraph::Drawable3D* { return mesh_data.drawable; },
                          [](const OpenglRenderable* renderable) -> SceneGraph::Drawable3D* {
                              return renderable->drawable();
                          },
                      },
                      data);
}

OpenglBackend::OpenglBackend() : framebuffer_(GL::defaultFramebuffer.viewport()) {
    using namespace Magnum;

    camera_object_.setParent(&scene_);
    camera_ = new SceneGraph::Camera3D(camera_object_); // Memory control is handled elsewhere

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    shader_.set_uniform_color({1.f, 0.5f, 0.1f});

    /*
     * Set up the framebuffer for intersection tests
     */
    id_rbo_.setStorage(GL::RenderbufferFormat::R32UI, GL::defaultFramebuffer.viewport().size());
    depth_rbo_.setStorage(GL::RenderbufferFormat::DepthComponent24, GL::defaultFramebuffer.viewport().size());

    framebuffer_.attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, id_rbo_)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, depth_rbo_)
        .mapForDraw({{GeneralShader::IdOutput, GL::Framebuffer::ColorAttachment{1}}});

    CORRADE_INTERNAL_ASSERT(framebuffer_.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
}

OpenglBackend::~OpenglBackend() = default;

auto OpenglBackend::render(CameraPackage const& camera_package) const -> void {
    using namespace Magnum;

    camera_object_.setTransformation(camera_package.camera->cameraMatrix().inverted());
    camera_->setProjectionMatrix(camera_package.camera->projectionMatrix());

    if (!custom_renderable_drawables_.isEmpty()) {
        camera_->draw(custom_renderable_drawables_);
    }

    if (!opaque_drawables_.isEmpty()) {
        camera_->draw(opaque_drawables_);
    }

    if (!wireframe_drawables_.isEmpty()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        camera_->draw(wireframe_drawables_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (!transparent_drawables_.isEmpty()) {
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        camera_->draw(transparent_drawables_);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    }

    // Don't draw the "non_visible_drawables_" obvs.

    // Draw to custom framebuffer
    framebuffer_.clearColor(GeneralShader::ColorOutput, Color3{0.125f})
        .clearColor(GeneralShader::IdOutput, Vector4ui{})
        .clearDepth(1.0f)
        .bind();

    if (!opaque_drawables_.isEmpty()) {
        camera_->draw(opaque_drawables_);
    }

    // Bind the main buffer back
    GL::defaultFramebuffer.bind();
}

auto OpenglBackend::resize(Vector2i const& viewport) -> void {
    framebuffer_.setViewport({{0u, 0u}, viewport});
}

auto OpenglBackend::added(SceneId const& item_id, SceneItemInfo const& item) -> void {

    auto* obj = (item_id == gvs::nil_id()) ? &scene_.addChild<Object3D>()
                                           : &get_item(item.parent).object->addChild<Object3D>();

    if (item.renderable) {
        if (auto* renderable = dynamic_cast<OpenglRenderable*>(item.renderable.get())) {
            add_item(item_id, obj, nullptr, renderable);
        } else {
            throw std::runtime_error("Backend/Renderable mismatch. "
                                     "`OpenglBackend` can only process `OpenglRenderable`s");
        }
        updated(item_id, UpdatedInfo::everything_but_geometry(), item);

    } else {
        add_item(item_id, obj, nullptr, shader_);
        updated(item_id, UpdatedInfo::everything(), item);
    }
}

auto OpenglBackend::updated(SceneId const& item_id, UpdatedInfo const& updated, SceneItemInfo const& item) -> void {
    OpenglItem& ogl_item = *id_to_pkgs_.at(item_id);

    if (std::holds_alternative<OpenglRenderable*>(ogl_item.data) && updated.geometry) {
        throw std::runtime_error("Geometry can not be updated when using a renderable");
    }

    if (auto* mesh_data = std::get_if<MeshData>(&ogl_item.data)) {

        if (updated.geometry) {
            auto const& geometry_info = item.geometry_info;

            if (updated.geometry_vertices) {
                update_vbo(mesh_data, geometry_info);
            }

            if (updated.geometry_indices) {
                update_ibo(mesh_data, geometry_info.indices);
            }
        }

        if (updated.display_geometry_format) {
            mesh_data->mesh.setPrimitive(to_magnum(item.display_info.geometry_format));
        }
    }

    // TODO: Handle parent and children updates properly

    if (updated.parent) {
        auto const& parent = item.parent;

        if (!util::has_key(id_to_pkgs_, parent)) {

            throw std::invalid_argument("Parent id '" + to_string(parent) + "' not found in scene");
        }

        ogl_item.object->setParent(id_to_pkgs_.at(parent)->object);
    }

    if (updated.display) {
        ogl_item.visible = item.display_info.visible;

        if (item.renderable) {
            ogl_item.drawable_group_when_visible = &custom_renderable_drawables_;

        } else if (item.display_info.wireframe_only) {
            ogl_item.drawable_group_when_visible = &wireframe_drawables_;

        } else if (item.display_info.opacity < 1.f) {
            ogl_item.drawable_group_when_visible = &transparent_drawables_;

        } else {
            ogl_item.drawable_group_when_visible = &opaque_drawables_;
        }

        std::visit(util::Visitor{
                       [&item](MeshData& mesh_data) { mesh_data.drawable->set_display_info(item.display_info); },
                       [&item](OpenglRenderable* renderable) { renderable->set_display_info(item.display_info); },
                   },
                   ogl_item.data);

        auto const& parent_ogl_item = *id_to_pkgs_.at(item.parent);
        auto const  parent_visible  = (parent_ogl_item.drawable()->group() != &non_visible_drawables_);

        update_drawable_group(&ogl_item, parent_visible);
    }
}

auto OpenglBackend::removed(SceneId const & /*item_id*/) -> void {}

auto OpenglBackend::reset_items(SceneItems const& items) -> void {
    // Remove all items from the scene
    if (util::has_key(id_to_pkgs_, gvs::nil_id())) {
        scene_.children().erase(get_item(gvs::nil_id()).object);
    }
    id_to_pkgs_.clear();
    obj_to_pkgs_.clear();
    packages_.clear();

    while (id_to_pkgs_.size() < items.size()) {
        for (auto const& [item_id, item] : items) {
            if (util::has_key(id_to_pkgs_, item_id)) {
                continue;
            }

            if (util::has_key(id_to_pkgs_, item.parent) || item_id == gvs::nil_id()) {
                added(item_id, item);
            }
        }
    }
}

auto OpenglBackend::add_item(SceneId id, Object3D* obj, SceneGraph::DrawableGroup3D* drawables, GeneralShader& shader)
    -> void {
    packages_.emplace_back(id, obj, drawables, next_intersect_id_++, shader);
    auto iter = std::prev(packages_.end());
    id_to_pkgs_.emplace(iter->scene_id, iter);
    obj_to_pkgs_.emplace(iter->object, iter);
    intersect_id_to_scene_id_.emplace(iter->intersect_id, iter->scene_id);
}

auto OpenglBackend::add_item(SceneId                      id,
                             Object3D*                    obj,
                             SceneGraph::DrawableGroup3D* drawables,
                             OpenglRenderable*            renderable) -> void {
    packages_.emplace_back(id, obj, drawables, next_intersect_id_++, renderable);
    auto iter = std::prev(packages_.end());
    id_to_pkgs_.emplace(iter->scene_id, iter);
    obj_to_pkgs_.emplace(iter->object, iter);
    intersect_id_to_scene_id_.emplace(iter->intersect_id, iter->scene_id);
}

auto OpenglBackend::remove_item(gvs::SceneId const& item_id) -> void {
    auto iter = id_to_pkgs_.at(item_id);
    id_to_pkgs_.erase(item_id);
    obj_to_pkgs_.erase(iter->object);
    packages_.erase(iter);
}

auto OpenglBackend::remove_item(Object3D const* obj) -> void {
    auto iter = obj_to_pkgs_.at(obj);
    obj_to_pkgs_.erase(obj);
    id_to_pkgs_.erase(iter->scene_id);
    packages_.erase(iter);
}

auto OpenglBackend::get_item(gvs::SceneId const& id) -> OpenglItem& {
    return *id_to_pkgs_.at(id);
}

auto OpenglBackend::get_item(gvs::SceneId const& id) const -> OpenglItem const& {
    return *id_to_pkgs_.at(id);
}

auto OpenglBackend::update_drawable_group(OpenglItem* ogl_item, bool parent_visible) -> void {

    auto update_children = [this, &ogl_item](bool parent_visibility) {
        for (auto const& child : ogl_item->object->children()) {
            auto& child_package = *obj_to_pkgs_.at(&child);
            update_drawable_group(&child_package, parent_visibility);
        }
    };

    auto currently_visible = (ogl_item->drawable()->group() != &non_visible_drawables_);

    if (!parent_visible) {
        if (currently_visible) {
            non_visible_drawables_.add(*ogl_item->drawable());
            update_children(false);
        }
        return;
    }

    // Currently visible but shouldn't be. Hide item and all children.
    if (currently_visible && !ogl_item->visible) {
        non_visible_drawables_.add(*ogl_item->drawable());
        update_children(false);
        return;
    }

    if (ogl_item->visible) {
        ogl_item->drawable_group_when_visible->add(*ogl_item->drawable());
        update_children(true);
    }
}

} // namespace ltb::gvs
