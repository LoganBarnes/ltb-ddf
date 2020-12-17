// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "graph_sub_view.hpp"
#include "ltb/gvs/display/gui/imgui_magnum_application.hpp"

namespace ltb::ddf::vis {

class MainWindow : public gvs::ImGuiMagnumApplication {
public:
    explicit MainWindow(const Arguments& arguments);
    ~MainWindow() override;

private:
    auto update() -> void final;
    auto render(gvs::CameraPackage const& camera_package) const -> void final;
    auto configure_gui() -> void final;

    auto resize(Magnum::Vector2i const& viewport) -> void final;

    auto handleKeyReleaseEvent(KeyEvent& event) -> void final;

    GraphSubView graph_sub_view_;
};

} // namespace ltb::ddf::vis
