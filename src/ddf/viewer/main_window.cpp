// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "main_window.hpp"

// project
#include "ltb/gvs/display/gui/docking.hpp"

using namespace Corrade;
using namespace Magnum;
using namespace Math::Literals;

namespace ltb::ddf::vis {

MainWindow::MainWindow(const Arguments& arguments)
    : ltb::gvs::ImGuiMagnumApplication(arguments,
                                       Configuration{}
                                           .setTitle("Dynamic Dataflow")
                                           .setSize({1280, 720})
                                           .setWindowFlags(Configuration::WindowFlag::Resizable)) {

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; // NOLINT(hicpp-signed-bitwise)

    resize(this->windowSize());
}

MainWindow::~MainWindow() = default;

auto MainWindow::update() -> void {}

auto MainWindow::render(ltb::gvs::CameraPackage const& /*camera_package*/) const -> void {}

auto MainWindow::configure_gui() -> void {

    gvs::configure_dockspace_and_menu_bar("Windows", {&graph_sub_view_}, [] {
        if (ImGui::BeginMenu("Settings")) {
            ImGui::EndMenu();
        }
    });

    error_alert_->display_next_error();
}

auto MainWindow::resize(Magnum::Vector2i const & /*viewport*/) -> void {}

auto MainWindow::handleKeyReleaseEvent(KeyEvent & /*event*/) -> void {}

} // namespace ltb::ddf::vis
