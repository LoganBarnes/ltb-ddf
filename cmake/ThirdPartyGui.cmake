##########################################################################################
# LTB Geometry Visualization Server
# Copyright (c) 2020 Logan Barnes - All Rights Reserved
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##########################################################################################
FetchContent_Declare(ltb_imgui_dl
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 06e87014f25ee2ac75661cff624dd2ef0a9b62ea # docking
        )
FetchContent_Declare(ltb_magnum_integration_dl
        GIT_REPOSITORY https://github.com/LoganBarnes/magnum-integration.git
        GIT_TAG 2b890def61c6cd2bb208375d5e662b29af01ab50 # docking
        )

ltb_add_external(gui Gui)

### ImGui ###
FetchContent_GetProperties(ltb_imgui_dl)
if (NOT ltb_imgui_dl_POPULATED)
    FetchContent_Populate(ltb_imgui_dl)
endif ()

### Magnum Integration ###
FetchContent_GetProperties(ltb_magnum_integration_dl)
if (NOT ltb_magnum_integration_dl_POPULATED)
    FetchContent_Populate(ltb_magnum_integration_dl)

    set(WITH_IMGUI ON CACHE BOOL "" FORCE)
    set(IMGUI_DIR "${ltb_imgui_dl_SOURCE_DIR}" CACHE STRING "" FORCE)

    add_subdirectory(${ltb_magnum_integration_dl_SOURCE_DIR} ${ltb_magnum_integration_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Tell cmake where to find this library
    set(MagnumIntegration_DIR ${ltb_magnum_integration_dl_SOURCE_DIR}/modules)

    find_package(MagnumIntegration REQUIRED ImGui)
endif ()

# Add the necessary libraries
target_link_libraries(ltb_external_gui
        INTERFACE
        MagnumIntegration::ImGui
        )
