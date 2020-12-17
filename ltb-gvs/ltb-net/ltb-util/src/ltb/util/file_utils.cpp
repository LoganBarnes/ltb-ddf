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
#include "file_utils.hpp"

// standard
#include <fstream>

namespace ltb::util {

auto read_file_to_string(const std::string& filename) -> util::Result<std::string> {
    std::ifstream input_stream(filename);

    if (!input_stream.is_open()) {
        return tl::make_unexpected(LTB_MAKE_ERROR("Failed to open: '" + filename + "'"));
    }

    input_stream.seekg(0, std::ios::end);
    auto size = input_stream.tellg();
    if (size == -1) {
        return {};
    }
    std::string buffer(static_cast<std::size_t>(size), ' ');
    input_stream.seekg(0);
    input_stream.read(&buffer[0], size);

    return buffer;
}

} // namespace ltb::util
