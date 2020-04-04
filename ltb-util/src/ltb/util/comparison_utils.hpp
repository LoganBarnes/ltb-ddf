// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Utilities
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#define GOOGLE_COMPARISON

#ifdef GOOGLE_COMPARISON
#include <google/AlmostEquals.h>
#else
#include <cmath>
#include <limits>
#endif

// standard
#include <type_traits>

namespace ltb::util {

#ifdef GOOGLE_COMPARISON

template <typename T>
auto constexpr almost_equal(T a, T b) -> typename std::enable_if_t<std::is_floating_point_v<T>, bool> {
    return AlmostEquals(a, b);
}

#else

template <typename T>
auto constexpr almost_equal(T a, T b, T scale = 1.0, T epsilon = 10 * std::numeric_limits<T>::epsilon()) ->
    typename std::enable_if_t<std::is_floating_point_v<T>, bool> {
    // exact comparison allows to handle infinities and prevents from getting FPE triggered by (inf - inf)
    const bool exactly_equal = (a == b);
    return exactly_equal || std::abs(a - b) < epsilon * (scale + std::abs(a) + std::abs(b));
}

#endif

} // namespace ltb::util
