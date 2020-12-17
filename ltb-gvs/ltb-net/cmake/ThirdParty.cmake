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

### Threads ###
find_package(Threads REQUIRED)

### Protobuf ###
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

if (${LTB_THREAD_SANITIZATION})
    add_compile_options(${LTB_SANITIZE_THREAD_FLAG})
    add_link_options(${LTB_SANITIZE_THREAD_FLAG})

    include(FetchContent)
    FetchContent_Declare(grpc_dl
            GIT_REPOSITORY https://github.com/grpc/grpc
            GIT_TAG v1.32.0
            )

    set(FETCHCONTENT_QUIET OFF)
    # FetchContent_MakeAvailable(grpc_dl)

    FetchContent_GetProperties(grpc_dl)
    if (NOT gRPC_POPULATED)
        FetchContent_Populate(grpc_dl)

        set(gRPC_INSTALL ON CACHE INTERNAL "Generate installation target")

        add_subdirectory(${grpc_dl_SOURCE_DIR} ${grpc_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

        # Mark headers as system headers to avoid compiler warnings
        target_include_directories(grpc++ SYSTEM PUBLIC $<BUILD_INTERFACE:${grpc_dl_SOURCE_DIR}/include>)
    endif ()

    message(STATUS "Using locally built gRPC")

    set(_GRPC_TARGET grpc++)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

else ()

    ### gRPC ###
    find_package(gRPC CONFIG REQUIRED)
    message(STATUS "Using gRPC ${gRPC_VERSION}")

    set(_GRPC_TARGET gRPC::grpc++)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif ()
