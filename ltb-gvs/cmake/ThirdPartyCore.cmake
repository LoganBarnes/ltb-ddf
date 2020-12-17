##########################################################################################
# LTB Geometry Visualization Server
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
include(FetchContent)

FetchContent_Declare(ltb_mapbox_dl
        GIT_REPOSITORY https://github.com/mapbox/variant.git
        GIT_TAG v1.1.6
        )

### Threads ###
find_package(Threads REQUIRED)

### Boost UUID ###
include(${CMAKE_CURRENT_LIST_DIR}/BoostUuidLibs.cmake)

### mapbox variant ###
FetchContent_GetProperties(ltb_mapbox_dl)
if (NOT ltb_mapbox_dl_POPULATED)
    FetchContent_Populate(ltb_mapbox_dl)

    ltb_add_external(mapbox Mapbox)
    target_include_directories(ltb_external_mapbox
            SYSTEM INTERFACE
            $<BUILD_INTERFACE:${ltb_mapbox_dl_SOURCE_DIR}/include>
            )
endif (NOT ltb_mapbox_dl_POPULATED)
