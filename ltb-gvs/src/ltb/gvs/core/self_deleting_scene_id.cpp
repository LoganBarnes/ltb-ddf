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
#include "self_deleting_scene_id.hpp"

namespace ltb {
namespace gvs {

SelfDeletingSceneID::SelfDeletingSceneID(Scene* /*scene*/, SceneId scene_id)
    : scene_id_(std::shared_ptr<SceneId>(new SceneId(scene_id), [/*scene*/](SceneId* scene_id_ptr) {
          // The shared pointer contains the scene removal code so we don't have to implement
          // a reference counter and special move/copy constructors.

          //          if (scene && *scene_id_ptr != nil_id) {
          //              scene->remove_item(*scene_id_ptr);
          //          }

          delete scene_id_ptr;
      })) {}

// The shared pointer contains the `scene->remove_item` functionality
SelfDeletingSceneID::~SelfDeletingSceneID() = default;

auto SelfDeletingSceneID::raw_id() const -> const SceneId& {
    return *scene_id_;
}

} // namespace gvs
} // namespace ltb
