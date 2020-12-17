// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/gvs/display/gui/sub_view.hpp"

// external
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_node_editor.h>

// standard
#include <memory>

namespace ed = ax::NodeEditor;

namespace ltb::ddf::vis {

class GraphSubView : public gvs::SubView {
public:
    explicit GraphSubView();
    ~GraphSubView() override;

    auto title() -> std::string override;
    auto configure_gui() -> void override;

    auto settings() const -> gvs::SubViewSettings override;

private:
    std::shared_ptr<ed::EditorContext> node_context_;
};

} // namespace ltb::ddf::vis
