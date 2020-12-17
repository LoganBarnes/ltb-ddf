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
FetchContent_Declare(ltb_glfw_dl
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.2
        )
FetchContent_Declare(ltb_corrade_dl
        GIT_REPOSITORY https://github.com/mosra/corrade.git
        GIT_TAG v2020.06
        )
FetchContent_Declare(ltb_magnum_dl
        GIT_REPOSITORY https://github.com/mosra/magnum.git
        GIT_TAG v2020.06
        )

# All external graphics libraries put into a single target
file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${LTB_GVS_ROOT}/external/*
        )

add_library(ltb_external_graphics ${LTB_SOURCE_FILES})
add_library(LtbExternal::Graphics ALIAS ltb_external_graphics)

if (MSVC)
    set(LTB_GVS_TEST_GL_CONTEXT WglContext)
    set(LTB_GVS_WINDOWLESS_APP WindowlessWglApplication)
else ()
    set(LTB_GVS_TEST_GL_CONTEXT EglContext)
    set(LTB_GVS_WINDOWLESS_APP WindowlessEglApplication)
endif ()

### GLFW ###
FetchContent_GetProperties(ltb_glfw_dl)
if (NOT ltb_glfw_dl_POPULATED)
    FetchContent_Populate(ltb_glfw_dl)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(${ltb_glfw_dl_SOURCE_DIR} ${ltb_glfw_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

### Corrade ###
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS OFF)

FetchContent_GetProperties(ltb_corrade_dl)
if (NOT ltb_corrade_dl_POPULATED)
    FetchContent_Populate(ltb_corrade_dl)

    if (MSVC_VERSION GREATER_EQUAL 1900)
        set(CORRADE_MSVC2019_COMPATIBILITY ON CACHE BOOL "" FORCE)
        set(MSVC2019_COMPATIBILITY ON CACHE BOOL "" FORCE)
    endif ()

    add_subdirectory(${ltb_corrade_dl_SOURCE_DIR} ${ltb_corrade_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Tell cmake where to find this library
    set(Corrade_DIR ${ltb_corrade_dl_SOURCE_DIR}/modules)
endif ()

### Magnum ###
FetchContent_GetProperties(ltb_magnum_dl)
if (NOT ltb_magnum_dl_POPULATED)
    FetchContent_Populate(ltb_magnum_dl)

    set(BUILD_DEPRECATED OFF CACHE BOOL "" FORCE)
    set(WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)

    if (MSVC)
        set(WITH_WGLCONTEXT ON CACHE BOOL "" FORCE)
        set(WITH_WINDOWLESSWGLAPPLICATION ON CACHE BOOL "" FORCE)
    else ()
        set(WITH_EGLCONTEXT ON CACHE BOOL "" FORCE)
        set(WITH_WINDOWLESSEGLAPPLICATION ON CACHE BOOL "" FORCE)
    endif ()

    add_subdirectory(${ltb_magnum_dl_SOURCE_DIR} ${ltb_magnum_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Tell cmake where to find this library
    set(Magnum_DIR ${ltb_magnum_dl_SOURCE_DIR}/modules)

    find_package(Magnum REQUIRED
            GL
            GlfwApplication
            ${LTB_GVS_TEST_GL_CONTEXT}
            ${LTB_GVS_WINDOWLESS_APP}
            )
endif ()

# Set the include directory as system headers to avoid compiler warnings
target_include_directories(ltb_external_graphics
        SYSTEM PUBLIC
        $<BUILD_INTERFACE:${LTB_GVS_ROOT}/external>
        $<BUILD_INTERFACE:${ltb_corrade_dl_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${ltb_magnum_dl_SOURCE_DIR}/src>
        )

# Add the necessary libraries
target_link_libraries(ltb_external_graphics
        PUBLIC
        Corrade::Utility
        Magnum::Application
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::Primitives
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        Magnum::${LTB_GVS_TEST_GL_CONTEXT}
        Magnum::${LTB_GVS_WINDOWLESS_APP}
        )
