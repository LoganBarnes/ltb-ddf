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
cmake_minimum_required(VERSION 3.15)
project(LtbUtilities LANGUAGES CXX)

include(cmake/ThirdParty.cmake)
include(cmake/LtbConfig.cmake) # <-- Additional project options are in here.

############
### Util ###
############
file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/util/*
        )

### Optional CUDA ###
if (CMAKE_CUDA_COMPILER)
    file(GLOB_RECURSE LTB_CUDA_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/src/ltb/cuda/*
            )
    list(APPEND LTB_SOURCE_FILES ${LTB_CUDA_SOURCE_FILES})
endif ()

ltb_add_library(ltb_util 17 ${LTB_SOURCE_FILES})
ltb_link_libraries(ltb_util
        PUBLIC
        expected
        range-v3
        Threads::Threads
        doctest
        )
ltb_include_directories(ltb_util
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>"
        )

