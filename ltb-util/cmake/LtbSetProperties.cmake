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
function(ltb_set_properties target cxx_standard)
    set_target_properties(
            ${target}
            PROPERTIES
            # C++ flags
            CXX_STANDARD ${cxx_standard}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
            # CUDA flags
            CUDA_STANDARD 14
            CUDA_STANDARD_REQUIRED ON
            CUDA_EXTENSIONS OFF
            CUDA_SEPARABLE_COMPILATION ON
            # CCache
            COMPILER_LAUNCHER "${LTB_CCACHE_PROGRAM}"
            # Clang-Tidy
            CXX_CLANG_TIDY "${LTB_CLANG_TIDY}"
    )
endfunction()
