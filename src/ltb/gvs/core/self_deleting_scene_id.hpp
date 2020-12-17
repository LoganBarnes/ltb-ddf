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
#include "scene.hpp"

// standard
#include <memory>

namespace ltb {
namespace gvs {

/// \todo Implement item removal from scenes before this can be used
struct SelfDeletingSceneID {
public:
    /// \brief Creates a type that deletes the given id from the provided scene on destruction.
    /// \param scene - the scene
    /// \param id - the id in the scene
    /// \warning {This scene pointer must remain valid for the lifetime of the SelfDeletingSceneID}
    explicit SelfDeletingSceneID(Scene* scene = nullptr, SceneId scene_id = nil_id());

    /// \brief Deletes the item from the scene if this is the last `SelfDeletingSceneID` for this item.
    ~SelfDeletingSceneID();

    auto raw_id() const -> const SceneId&;

private:
    std::shared_ptr<SceneId> scene_id_ = nullptr;
};

} // namespace gvs
} // namespace ltb
