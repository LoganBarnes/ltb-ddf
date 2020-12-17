##########################################################################################
# LTB Networking
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
cmake_minimum_required(VERSION 3.18)
project(LtbNet LANGUAGES CXX)

option(LTB_NET_BUILD_EXAMPLES "Build an example server and client" OFF)

include(${CMAKE_CURRENT_LIST_DIR}/ltb-util/cmake/LtbConfig.cmake) # <-- Additional project options are in here.

############
### Util ###
############
add_subdirectory(ltb-util)

########################
### External Targets ###
########################
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ThirdParty.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CreateProtoLibrary.cmake)

####################
### Source Files ###
####################
file(GLOB_RECURSE LTB_CORE_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/net/*
        )

#######################
#### LTB Networking ###
#######################
ltb_add_library(ltb_net
        17
        ${LTB_CORE_SOURCE_FILES}
        $<$<BOOL:${LTB_BUILD_TESTS}>:${LTB_TEST_SOURCE_FILES}>
        )

ltb_link_libraries(ltb_net
        PUBLIC
        ${_GRPC_TARGET}
        Ltb::Util
        )

ltb_include_directories(ltb_net
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
        )

add_library(Ltb::Net ALIAS ltb_net)

################
#### Testing ###
################
if (${LTB_BUILD_TESTS})
    create_proto_library(ltb_net_testing_protos
            ${CMAKE_CURRENT_LIST_DIR}/protos/testing
            ${CMAKE_BINARY_DIR}/generated/protos
            )

    target_link_libraries(test_ltb_net PRIVATE ltb_net_testing_protos)
endif ()

###############
### Example ###
###############
if (${LTB_NET_BUILD_EXAMPLES})
    add_subdirectory(example)
endif ()
