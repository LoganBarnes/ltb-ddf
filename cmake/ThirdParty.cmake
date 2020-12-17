##########################################################################################
# Copyright (c) 2020 Logan Barnes - All Rights Reserved
##########################################################################################
FetchContent_Declare(ltb_imgui_node_editor_dl
        GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor.git
        GIT_TAG 687a72f940c76cf5064e13fe55fa0408c18fcbe4 # master
        )
FetchContent_Declare(ltb_flexcore_dl
        GIT_REPOSITORY https://github.com/LoganBarnes/flexcore.git
        GIT_TAG 7c0e465e73cc132a18340e9bdee8027ef058fb65 # master
        )

### ImGui Node Editor ###
FetchContent_GetProperties(ltb_imgui_node_editor_dl)
if (NOT ltb_imgui_node_editor_dl_POPULATED)
    FetchContent_Populate(ltb_imgui_node_editor_dl)

    add_library(imgui_node_editor
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/crude_json.cpp
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/crude_json.h
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_bezier_math.h
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_bezier_math.inl
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_canvas.cpp
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_canvas.h
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_extra_math.h
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_extra_math.inl
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_node_editor.cpp
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_node_editor.h
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_node_editor_api.cpp
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_node_editor_internal.h
            ${ltb_imgui_node_editor_dl_SOURCE_DIR}/imgui_node_editor_internal.inl
            )
    target_link_libraries(imgui_node_editor
            PUBLIC MagnumIntegration::ImGui
            )
    target_include_directories(imgui_node_editor
            SYSTEM PUBLIC
            $<BUILD_INTERFACE:${ltb_imgui_node_editor_dl_SOURCE_DIR}>
            )
    target_compile_options(imgui_node_editor
            PRIVATE
            $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-w>
            )
    add_library(LtbExternal::ImGuiNodeEditor ALIAS imgui_node_editor)
endif (NOT ltb_imgui_node_editor_dl_POPULATED)

### Flexcore ###
FetchContent_GetProperties(ltb_flexcore_dl)
if (NOT ltb_flexcore_dl_POPULATED)
    FetchContent_Populate(ltb_flexcore_dl)

    find_package(Threads REQUIRED)
    find_package(Boost 1.65 REQUIRED COMPONENTS log)

    add_library(flexcore
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/infrastructure.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/extended/graph/graph.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/utils/logging/logger.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/utils/demangle.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/extended/base_node.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/extended/visualization/visualization.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/scheduler/clock.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/scheduler/cyclecontrol.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/scheduler/parallelregion.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/scheduler/parallelscheduler.cpp
            ${ltb_flexcore_dl_SOURCE_DIR}/flexcore/scheduler/serialschedulers.cpp
            )
    target_include_directories(flexcore
            SYSTEM PUBLIC
            $<BUILD_INTERFACE:${ltb_flexcore_dl_SOURCE_DIR}/>
            $<BUILD_INTERFACE:${ltb_flexcore_dl_SOURCE_DIR}/flexcore/3rdparty>
            $<BUILD_INTERFACE:${ltb_flexcore_dl_SOURCE_DIR}/flexcore/3rdparty/cereal/include>
            )
    target_compile_options(flexcore
                PRIVATE
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-w>
                )
    target_link_libraries(flexcore
            PUBLIC
            Boost::boost
            Boost::log
            Threads::Threads
            )
    set_target_properties(
            flexcore
            PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
    )

    add_library(LtbExternal::Flexcore ALIAS flexcore)
endif (NOT ltb_flexcore_dl_POPULATED)
