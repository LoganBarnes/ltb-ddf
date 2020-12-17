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
#include "sub_view.hpp"

// standard
#include <functional>
#include <vector>

namespace ltb::gvs {

/// \brief Creates a dockspace and displays a list of `SubView`s. A menu bar will also be
///        created so all available `SubView`s can be shown and toggled.
/// \param menu_title - the title of the menu item corresponding to the SubViews
/// \param sub_views - the list of dockable `SubView` windows
/// \param additional_menu_configuration -
///        this function is called between `ImGui::BeginMainMenuBar` and
///        `ImGui::EndMainMenuBar` and can be used to add more items to the menu bar.
auto configure_dockspace_and_menu_bar(const std::string&           sub_view_menu_title,
                                      const std::vector<SubView*>& sub_views,
                                      const std::function<void()>& additional_menu_configuration = nullptr) -> void;

} // namespace ltb::gvs
