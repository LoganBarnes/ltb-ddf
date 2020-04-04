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

    option(LTB_BUILD_TESTS "Build unit tests" OFF)
    option(LTB_USE_DEV_FLAGS "Compile with all the flags" OFF)

    # Disabling CUDA support for lower versions because there is a cmake bug
    # which causes an undefined reference to '__cudaUnregisterFatBinary'.
    if (${CMAKE_VERSION} VERSION_GREATER "3.16.2")
        include(CheckLanguage)

        check_language(CUDA)
        if (CMAKE_CUDA_COMPILER)
            enable_language(CUDA)
            set(LTB_CUDA_ENABLED ON)
        endif ()
    endif ()

    if (MSVC)
        add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # silly microsoft
    endif ()

    if (LTB_USE_DEV_FLAGS)
        if (NOT MSVC)
            set(LTB_COMPILE_FLAGS
                    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
                    $<$<COMPILE_LANGUAGE:CXX>:-Werror>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wunused>
                    $<$<COMPILE_LANGUAGE:CXX>:-pedantic-errors>
                    $<$<COMPILE_LANGUAGE:CXX>:-Winit-self>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
                    $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-declarations>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-include-dirs>
                    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unknown-pragmas>
                    )
        else ()
            set(LTB_COMPILE_FLAGS
                    $<$<COMPILE_LANGUAGE:CXX>:/WX>
                    $<$<COMPILE_LANGUAGE:CXX>:/experimental:external>
                    $<$<COMPILE_LANGUAGE:CXX>:/external:anglebrackets>
                    $<$<COMPILE_LANGUAGE:CXX>:/external:W0>
                    )
        endif ()
    endif ()

    add_library(ltb_testing INTERFACE)

    include(${CMAKE_CURRENT_LIST_DIR}/CCache.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/ClangTidy.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbSetProperties.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbAddLibrary.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/LtbAddExecutable.cmake)

    if (LTB_BUILD_TESTS)
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

    if (MSVC)
        set(SLASH "\\\\")
    else ()
        set(SLASH "/")
    endif ()

    configure_file(${CMAKE_CURRENT_LIST_DIR}/../src/ltb_paths.hpp.in ${CMAKE_BINARY_DIR}/generated/ltb/paths.hpp)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../src/ltb_config.hpp.in ${CMAKE_BINARY_DIR}/generated/ltb/config.hpp)

    set(LTB_CONFIGURED TRUE)
endif (NOT LTB_CONFIGURED)
