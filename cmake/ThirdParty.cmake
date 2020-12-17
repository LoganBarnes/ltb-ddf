##########################################################################################
# Copyright (c) 2020 Logan Barnes - All Rights Reserved
##########################################################################################
FetchContent_Declare(
        ltb_imgui_node_editor_dl
        GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor.git
        GIT_TAG 687a72f940c76cf5064e13fe55fa0408c18fcbe4 # master
)

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
            PUBLIC
            $<BUILD_INTERFACE:${ltb_imgui_node_editor_dl_SOURCE_DIR}>
            )
    target_compile_options(imgui_node_editor
            PRIVATE
            $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-w>
            )
    add_library(LtbExternal::ImGuiNodeEditor ALIAS imgui_node_editor)
endif (NOT ltb_imgui_node_editor_dl_POPULATED)
