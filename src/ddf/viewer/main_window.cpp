// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "main_window.hpp"

// project

// external
#include <Magnum/Math/Matrix.h>

// standard
#include <iostream>

using namespace Corrade;
using namespace Magnum;
using namespace Math::Literals;

namespace ltb::ddf::vis {

MainWindow::MainWindow(const Arguments& arguments)
    : ltb::gvs::ImGuiMagnumApplication(arguments,
                                       Configuration{}
                                           .setTitle("Dynamic Dataflow")
                                           .setSize({0, 0})
                                           .setWindowFlags(Configuration::WindowFlag::Resizable)) {
    resize(this->windowSize());
}

MainWindow::~MainWindow() = default;

auto MainWindow::update() -> void {}

auto MainWindow::render(ltb::gvs::CameraPackage const& /*camera_package*/) const -> void {}

auto MainWindow::configure_gui() -> void {
    error_alert_->display_next_error();
}

auto MainWindow::resize(Magnum::Vector2i const& /*viewport*/) -> void {}

auto MainWindow::handleKeyReleaseEvent(KeyEvent& /*event*/) -> void {}

} // namespace ltb::ddf::vis
