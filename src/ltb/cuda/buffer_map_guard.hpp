// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
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

#include "gl_buffer.hpp"
#include "gl_buffer_image.hpp"

namespace ltb {
namespace cuda {

template <typename T>
class GLBufferMapGuard {
public:
    explicit GLBufferMapGuard(GLBuffer<T>& interop_buffer) : interop_buffer_(interop_buffer) {
        interop_buffer.map_for_cuda();
    }
    ~GLBufferMapGuard() { interop_buffer_.unmap_from_cuda(); }

private:
    GLBuffer<T>& interop_buffer_;
};

// Allows the guard to be created without having to specify the template type
template <typename T>
GLBufferMapGuard<T> make_gl_buffer_map_guard(GLBuffer<T>& interop_buffer) {
    return GLBufferMapGuard<T>(interop_buffer);
}

template <typename T>
class GLBufferImageMapGuard {
public:
    explicit GLBufferImageMapGuard(GLBufferImage<T>& interop_buffer) : interop_buffer_(interop_buffer) {
        interop_buffer.map_for_cuda();
    }
    ~GLBufferImageMapGuard() { interop_buffer_.unmap_from_cuda(); }

private:
    GLBufferImage<T>& interop_buffer_;
};

// Allows the guard to be created without having to specify the template type
template <typename T>
GLBufferImageMapGuard<T> make_gl_buffer_map_guard(GLBufferImage<T>& interop_buffer) {
    return GLBufferImageMapGuard<T>(interop_buffer);
}

} // namespace cuda
} // namespace ltb
