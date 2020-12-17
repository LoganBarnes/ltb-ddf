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
include(FetchContent)

FetchContent_Declare(ltb_expected_dl
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG v1.0.0
        )
FetchContent_Declare(ltb_range_v3_dl
        GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
        GIT_TAG 0.11.0
        )
FetchContent_Declare(ltb_doctest_dl
        GIT_REPOSITORY https://github.com/onqtam/doctest.git
        GIT_TAG 2.3.6
        )

### Expected ###
FetchContent_GetProperties(ltb_expected_dl)
if (NOT ltb_expected_dl_POPULATED)
    FetchContent_Populate(ltb_expected_dl)

    set(EXPECTED_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(EXPECTED_ENABLE_DOCS OFF CACHE BOOL "" FORCE)

    # compile expected with current project
    add_subdirectory(${ltb_expected_dl_SOURCE_DIR} ${ltb_expected_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    ltb_add_external(expected Expected)
    target_link_libraries(ltb_external_expected
            INTERFACE
            expected
            )
endif (NOT ltb_expected_dl_POPULATED)

### Range-v3 ###
FetchContent_GetProperties(ltb_range_v3_dl)
if (NOT ltb_range_v3_dl_POPULATED)
    FetchContent_Populate(ltb_range_v3_dl)

    set(RANGES_CXX_STD 17 CACHE INTERNAL "C++ standard version")

    # compile with current project
    add_subdirectory(${ltb_range_v3_dl_SOURCE_DIR} ${ltb_range_v3_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    ltb_add_external(range_v3 RangeV3)
    target_include_directories(ltb_external_range_v3
            SYSTEM INTERFACE
            $<BUILD_INTERFACE:${ltb_range_v3_dl_SOURCE_DIR}/include/>
            )
    target_compile_options(ltb_external_range_v3
            INTERFACE
            $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->
            $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/std:c++latest>
            )
endif (NOT ltb_range_v3_dl_POPULATED)

### DocTest ###
FetchContent_GetProperties(ltb_doctest_dl)
if (NOT ltb_doctest_dl_POPULATED)
    FetchContent_Populate(ltb_doctest_dl)

    set(DOCTEST_WITH_TESTS OFF CACHE BOOL "" FORCE)
    set(DOCTEST_WITH_MAIN_IN_STATIC_LIB ON CACHE BOOL "" FORCE)

    # compile with current project
    add_subdirectory(${ltb_doctest_dl_SOURCE_DIR} ${ltb_doctest_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    ltb_add_external(doctest Doctest)
    target_link_libraries(ltb_external_doctest
            INTERFACE
            doctest
            )
    ltb_add_external(doctest_with_main DoctestWithMain)
    target_link_libraries(ltb_external_doctest_with_main
            INTERFACE
            doctest_with_main
            )

    # add test coverage capabilities if available
    find_program(LCOV_EXE
            NAMES "lcov"
            DOC "Path to lcov executable"
            )

    if (LCOV_EXE AND CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE MATCHES "Debug")
        include(${CMAKE_CURRENT_LIST_DIR}/CodeCoverage.cmake)
    endif ()
endif ()
