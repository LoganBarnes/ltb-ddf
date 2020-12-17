##########################################################################################
# LTB Utilities
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

#############################
### Project Configuration ###
#############################
if (NOT LTB_CONFIGURED)

    function(ltb_configure_file input_file output_file)
        file(READ ${input_file} INPUT_FILE_STR)
        string(CONFIGURE "${INPUT_FILE_STR}" CONFIGURED_FILE_STR)
        file(GENERATE OUTPUT ${output_file} CONTENT "${CONFIGURED_FILE_STR}")
    endfunction()

    option(LTB_BUILD_TESTS "Build unit tests" OFF)
    option(LTB_USE_DEV_FLAGS "Compile with all the flags" OFF)
    option(LTB_USE_CUDA "Enable CUDA features if available" ON)
    option(LTB_THREAD_SANITIZATION "Add thread sanitizer flags (clang debug only)" OFF)

    # Disabling CUDA support for lower versions because there is a cmake bug
    # which causes an undefined reference to '__cudaUnregisterFatBinary'.
    if (LTB_USE_CUDA AND ${CMAKE_VERSION} VERSION_GREATER "3.16.2")
        include(CheckLanguage)

        check_language(CUDA)
        if (${CMAKE_CUDA_COMPILER})
            enable_language(CUDA)
            set(LTB_CUDA_ENABLED ON)
        endif ()
    endif ()

    if (MSVC)
        add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # silly microsoft
    endif ()

    string(CONCAT LTB_SANITIZE_THREAD_FLAG
            "$<"
            "$<AND:"
            "$<BOOL:${LTB_THREAD_SANITIZATION}>,"
            "$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>,"
            "$<CONFIG:DEBUG>"
            ">:"
            "-fsanitize=thread"
            ">")

    set(LTB_COMPILE_FLAGS
            # Ignore any headers using angle brackets on windows
            $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/experimental:external>
            $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:anglebrackets>
            $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:W0>
            ${LTB_SANITIZE_THREAD_FLAG}
            )

    set(LTB_LINK_FLAGS ${LTB_SANITIZE_THREAD_FLAG})

    if (${LTB_USE_DEV_FLAGS})
        list(APPEND
                LTB_COMPILE_FLAGS
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wall>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wextra>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Werror>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wpedantic>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wunused>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-pedantic-errors>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Winit-self>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wold-style-cast>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Woverloaded-virtual>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wsign-conversion>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wshadow>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wmissing-declarations>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wmissing-include-dirs>
                $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wno-unknown-pragmas>
                $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:W0>
                )
    endif ()

    add_library(ltb_testing INTERFACE)

    include(${CMAKE_CURRENT_LIST_DIR}/CCache.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/ClangTidy.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbSetProperties.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbAddLibrary.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbAddExecutable.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbAddExternal.cmake)

    if (${LTB_BUILD_TESTS})
        enable_testing()
    endif ()

    # "Glob is terrible/root of all evil" yeah yeah. CONFIGURE_DEPENDS in cmake 3.12
    # helps to fix that and it is super useful when refactoring
    cmake_policy(SET CMP0009 NEW)

    set(AUTO_GENERATION_MESSAGE "***This file is auto generated. If you modify it the changes will be overwritten.***")

    # Fix path depending on platform
    # TODO: Use std::filesystem when it is available on most compilers so we don't have to deal with this
    file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}" NATIVE_PROJECT_SOURCE_DIR)
    string(REPLACE "\\" "\\\\" NATIVE_PROJECT_SOURCE_DIR ${NATIVE_PROJECT_SOURCE_DIR})

    file(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}" NATIVE_PROJECT_BINARY_DIR)
    string(REPLACE "\\" "\\\\" NATIVE_PROJECT_BINARY_DIR ${NATIVE_PROJECT_BINARY_DIR})

    if (MSVC)
        set(SLASH "\\\\")
    else ()
        set(SLASH "/")
    endif ()

    ltb_configure_file(
            ${CMAKE_CURRENT_LIST_DIR}/../src/ltb_paths.hpp.in
            ${CMAKE_BINARY_DIR}/generated/ltb/paths.hpp
    )
    ltb_configure_file(
            ${CMAKE_CURRENT_LIST_DIR}/../src/ltb_config.hpp.in
            ${CMAKE_BINARY_DIR}/generated/ltb/config.hpp
    )

    set(LTB_CONFIGURED TRUE)
endif (NOT LTB_CONFIGURED)
