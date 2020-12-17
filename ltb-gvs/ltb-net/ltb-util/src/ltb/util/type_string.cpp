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
#include "type_string.hpp"

#include <regex>
#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>

namespace ltb::util::detail {

auto demangle(const char* name) -> std::string {
    int                                    status = {};
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
    return (status == 0) ? res.get() : name;
}

#else

namespace ltb::util::detail {

// does nothing if not g++
std::string demangle(const char* name) {
    return name;
}

#endif

namespace {

constexpr const char* ugly_string1
    = "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >";

constexpr const char* ugly_string2 = "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >";

} // namespace

auto replace_ugly_strings(std::string type_str) -> std::string {
    type_str = std::regex_replace(type_str, std::regex(ugly_string1), std::string("std::string>"));
    type_str = std::regex_replace(type_str, std::regex(ugly_string2), std::string("std::string"));
    return type_str;
}

} // namespace ltb::util
