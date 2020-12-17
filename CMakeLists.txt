##########################################################################################
# Copyright (c) 2020 Logan Barnes - All Rights Reserved
##########################################################################################
cmake_minimum_required(VERSION 3.18)
project(LtbDynamicDataflow LANGUAGES CXX)

# Download external repos into a shared directory outside
# the build folders to prevent multiple repeat downloads
set(FETCHCONTENT_BASE_DIR
        ${CMAKE_SOURCE_DIR}/cmake-build-deps
        CACHE PATH ""
        )

include(${CMAKE_CURRENT_LIST_DIR}/ltb-gvs/ltb-net/ltb-util/cmake/LtbConfig.cmake)
# Additional project options are in here ^

### GVS ###
set(LTB_GVS_ENABLE_NETWORKING "OFF" CACHE INTERNAL "")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ltb-gvs)

####################
#### Third Party ###
####################
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ThirdParty.cmake)

#####################
#### Source Files ###
#####################
file(GLOB_RECURSE LTB_DDF_VIS_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ddf/viewer/*
        )
file(GLOB_RECURSE LTB_DDF_CORE_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ddf/core/*
        )
set(LTB_DDF_VIS_MAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/src/ddf/viewer/main.cpp)
list(REMOVE_ITEM LTB_DDF_VIS_SOURCE_FILES ${LTB_DDF_VIS_MAIN_FILE})

################
### DDF Core ###
################
ltb_add_library(ltb_ddf_core
        17
        ${LTB_DDF_CORE_SOURCE_FILES}
        )
ltb_link_libraries(ltb_ddf_core
        PUBLIC
        Ltb::Util
        )
include_directories(ltb_ddf_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
        )
add_library(Ltb::Ddf ALIAS ltb_ddf_core)

##################
### DDF Viewer ###
##################
ltb_add_executable(ltb_ddf_viewer
        17
        ${LTB_DDF_VIS_MAIN_FILE}
        ${LTB_DDF_VIS_SOURCE_FILES}
        )
ltb_link_libraries(ltb_ddf_viewer
        PUBLIC
        Ltb::Gvs
        Ltb::Ddf
        LtbExternal::ImGuiNodeEditor
        LtbExternal::Flexcore
        )
add_library(Ltb::DdfViewer ALIAS ltb_ddf_viewer)

################
### Examples ###
################
if (${LTB_DDF_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif ()
