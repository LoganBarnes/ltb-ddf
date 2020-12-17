##########################################################################################
# gRPC Wrapper
# Copyright (c) 2019 Logan Barnes - All Rights Reserved
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
function(grpc_generate_cpp _generated_files)
    # set(options OPTIONAL FAST)
    set(oneValueArgs CPP_OUT GRPC_OUT)
    set(multiValueArgs IMPORT_DIRS PROTO_FILES)
    cmake_parse_arguments(GRPC_GEN_CPP "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    list(LENGTH GRPC_GEN_CPP_IMPORT_DIRS IMPORT_DIRS_COUNT)

    if (${IMPORT_DIRS_COUNT} EQUAL 0)
        set(PRIMARY_IMPORT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else ()
        list(GET GRPC_GEN_CPP_IMPORT_DIRS 0 GRPC_GEN_CPP_IMPORT_DIRS_0)
        get_filename_component(PRIMARY_IMPORT_DIR ${GRPC_GEN_CPP_IMPORT_DIRS_0} ABSOLUTE)
    endif ()

    foreach (IMPORT_DIR ${GRPC_GEN_CPP_IMPORT_DIRS})
        get_filename_component(ABSOLUTE_IMPORT_PATH ${IMPORT_DIR} ABSOLUTE)
        list(APPEND _IMPORT_DIRS -I ${ABSOLUTE_IMPORT_PATH})
    endforeach ()

    foreach (PROTO_FILE ${GRPC_GEN_CPP_PROTO_FILES})
        get_filename_component(ABSOLUTE_PROTO_FILE ${PROTO_FILE} ABSOLUTE)
        list(APPEND _PROTO_FILES ${ABSOLUTE_PROTO_FILE})

        get_filename_component(PROTO_FILE_DIRECTORY ${PROTO_FILE} DIRECTORY)
        get_filename_component(PROTO_FILE_NAME_WE ${PROTO_FILE} NAME_WE)
        list(APPEND _PB_GENERATED_FILES
                ${PROTO_FILE_DIRECTORY}/${PROTO_FILE_NAME_WE}.pb.h
                ${PROTO_FILE_DIRECTORY}/${PROTO_FILE_NAME_WE}.pb.cc
                )
        list(APPEND _GRPC_PB_GENERATED_FILES
                ${PROTO_FILE_DIRECTORY}/${PROTO_FILE_NAME_WE}.grpc.pb.h
                ${PROTO_FILE_DIRECTORY}/${PROTO_FILE_NAME_WE}.grpc.pb.cc
                )
    endforeach ()

    list(TRANSFORM _PB_GENERATED_FILES REPLACE "${PRIMARY_IMPORT_DIR}" "${GRPC_GEN_CPP_CPP_OUT}")
    list(TRANSFORM _GRPC_PB_GENERATED_FILES REPLACE "${PRIMARY_IMPORT_DIR}" "${GRPC_GEN_CPP_GRPC_OUT}")

    list(APPEND
            _GENERATED_FILES
            ${_PB_GENERATED_FILES}
            ${_GRPC_PB_GENERATED_FILES}
            )

    add_custom_command(
            OUTPUT ${_GENERATED_FILES}
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS
            --cpp_out ${GRPC_GEN_CPP_CPP_OUT} # <file>.pb.h/cc
            --grpc_out ${GRPC_GEN_CPP_GRPC_OUT} # <file>.grpc.pb.h/cc
            ${_IMPORT_DIRS}
            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
            ${_PROTO_FILES}
            DEPENDS ${_PROTO_FILES}
            VERBATIM
    )

    set_source_files_properties(${_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    set(${_generated_files} ${_GENERATED_FILES} PARENT_SCOPE)
endfunction()

function(create_proto_library _target_name _proto_dir _generated_dir)

    # make the output directory
    file(MAKE_DIRECTORY ${_generated_dir})

    # gather proto files
    file(GLOB_RECURSE PROTO_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${_proto_dir}/*.proto
            )

    # generate service objects
    grpc_generate_cpp(GENERATED_FILES
            CPP_OUT ${_generated_dir}
            GRPC_OUT ${_generated_dir}
            IMPORT_DIRS ${_proto_dir}
            PROTO_FILES ${PROTO_FILES}
            )

    add_library(${_target_name} ${GENERATED_FILES})
    target_link_libraries(${_target_name} PUBLIC ${_GRPC_TARGET})
    target_include_directories(${_target_name} SYSTEM PUBLIC ${_generated_dir})
endfunction()
