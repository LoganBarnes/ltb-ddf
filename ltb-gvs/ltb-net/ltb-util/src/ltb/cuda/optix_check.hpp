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

// external
#include <optix_stubs.h>

// standard
#include <exception>
#include <sstream>
#include <string>

#define LTB_OPTIX_CHECK(val) ::ltb::optix::check((val), #val, __FILE__, __LINE__)

#define LTB_SAFE_OPTIX_CHECK(val)                                                                                      \
    {                                                                                                                  \
        auto error_message = ::ltb::optix::error_string((val), #val, __FILE__, __LINE__);                              \
        if (!error_message.empty()) {                                                                                  \
            return tl::make_unexpected(LTB_MAKE_ERROR(error_message));                                                 \
        }                                                                                                              \
    }

namespace ltb {
namespace optix {

template <typename T>
auto error_string(T result, char const* const func, const char* const file, int const line) -> std::string {
    if (result != OPTIX_SUCCESS) {
        std::stringstream error_str;
        error_str << "OPTIX error at " << file << ":" << line << "\n";
        error_str << "\tcode=" << static_cast<unsigned int>(result) << "(" << optixGetErrorString(result) << ")\n";
        error_str << "\t\"" << func << "\"";
        return error_str.str();
    }
    return "";
}

template <typename T>
auto check(T result, char const* const func, const char* const file, int const line) -> void {
    auto error_message = error_string(result, func, file, line);
    if (!error_message.empty()) {
        throw std::runtime_error((error_message));
    }
}

} // namespace optix
} // namespace ltb
