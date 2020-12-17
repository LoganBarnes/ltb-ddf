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
#include "docking.hpp"

// project
#include "imgui_colors.hpp"
#include "imgui_utils.hpp"

namespace ltb::gvs {

auto configure_dockspace_and_menu_bar(const std::string&           sub_view_menu_title,
                                      const std::vector<SubView*>& sub_views,
                                      const std::function<void()>& additional_menu_configuration) -> void {

    /*
     * Full screen docking window settings
     */
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) { // NOLINT(hicpp-signed-bitwise)
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    /*
     * Main window menu bar
     */
    if (ImGui::BeginMainMenuBar()) {
        if (sub_views.empty()) {
            ImGui::TextColored(light_gray(), "%s (None)", sub_view_menu_title.c_str());

        } else if (ImGui::BeginMenu(sub_view_menu_title.c_str())) {
            for (auto* sub_view : sub_views) {
                ScopedID imgui_id(sub_view);

                auto title = sub_view->title();

                if (ImGui::MenuItem(title.c_str(), "", sub_view->open())) {
                    sub_view->set_open(!sub_view->open());
                }
            }
            ImGui::EndMenu();
        }

        if (additional_menu_configuration) {
            additional_menu_configuration();
        }
        ImGui::EndMainMenuBar();
    }

    /*
     * Display all open views
     */
    for (auto* sub_view : sub_views) {
        ScopedID imgui_id(sub_view);
        sub_view->display();
    }
}

} // namespace ltb::gvs
