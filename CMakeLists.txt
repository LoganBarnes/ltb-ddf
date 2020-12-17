##########################################################################################
# LTB Dynamic Dataflow
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
cmake_minimum_required(VERSION 3.16.5)
project(LtbDynamicDataflow LANGUAGES CXX)

option(LTB_DDF_BUILD_EXAMPLES "Build example programs" OFF)

include(ltb-util/cmake/LtbConfig.cmake) # <-- Additional project options are in here.

############
### Util ###
############
add_subdirectory(ltb-util)

###########
### DDF ###
###########
add_library(ltb_ddf INTERFACE)

################
### DDF Core ###
################
file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ddf/core/*
        )

ltb_add_library(ltb_dvh_core 17 ${LTB_SOURCE_FILES})
ltb_link_libraries(ltb_dvh_core
        PUBLIC
        ltb_util
        )
include_directories(ltb_dvh_core
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
        )

target_link_libraries(ltb_ddf INTERFACE ltb_dvh_core)

if (${LTB_DDF_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif ()
