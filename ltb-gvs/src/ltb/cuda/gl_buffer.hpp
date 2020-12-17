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

// project
#include "ltb/cuda/cuda_check.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Buffer.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

// standard
#include <vector>

namespace ltb {
namespace cuda {

/// @brief Maps an allocated OpenGL buffer for use with CUDA
template <typename T>
class GLBuffer {
public:
    explicit GLBuffer(Magnum::GL::Buffer gl_buffer = Magnum::GL::Buffer{});
    ~GLBuffer();

    auto set_stream(cudaStream_t stream) -> void;

    auto map_for_cuda() -> void;
    auto unmap_from_cuda() -> void;

    auto cuda_buffer(std::size_t* size = nullptr) const -> T*;

    [[nodiscard]] auto gl_buffer() const -> Magnum::GL::Buffer const&;
    auto               gl_buffer() -> Magnum::GL::Buffer&;

private:
    Magnum::GL::Buffer     gl_buffer_;
    cudaGraphicsResource_t graphics_resource_ = nullptr;
    cudaStream_t           stream_            = nullptr;
    bool                   mapped_for_cuda_   = false;
};

template <typename T>
GLBuffer<T>::GLBuffer(Magnum::GL::Buffer gl_buffer) : gl_buffer_(std::move(gl_buffer)) {
    LTB_CUDA_CHECK(cudaGraphicsGLRegisterBuffer(&graphics_resource_, gl_buffer_.id(), cudaGraphicsRegisterFlagsNone));
}

template <typename T>
GLBuffer<T>::~GLBuffer() {
    LTB_CUDA_CHECK(cudaGraphicsUnregisterResource(graphics_resource_));
}

template <typename T>
auto GLBuffer<T>::set_stream(cudaStream_t stream) -> void {
    stream_ = stream;
}

template <typename T>
auto GLBuffer<T>::map_for_cuda() -> void {
    if (!mapped_for_cuda_) {
        LTB_CUDA_CHECK(cudaGraphicsMapResources(1, &graphics_resource_, stream_));
        mapped_for_cuda_ = true;
    }
}

template <typename T>
auto GLBuffer<T>::unmap_from_cuda() -> void {
    if (mapped_for_cuda_) {
        LTB_CUDA_CHECK(cudaGraphicsUnmapResources(1, &graphics_resource_, stream_));
        mapped_for_cuda_ = false;
    }
}

template <typename T>
auto GLBuffer<T>::cuda_buffer(std::size_t* size) const -> T* {
    T*          dev_ptr   = nullptr;
    std::size_t byte_size = 0;
    LTB_CUDA_CHECK(
        cudaGraphicsResourceGetMappedPointer(reinterpret_cast<void**>(&dev_ptr), &byte_size, graphics_resource_));
    if (size) {
        *size = byte_size / sizeof(T);
    }
    return dev_ptr;
}

template <typename T>
auto GLBuffer<T>::gl_buffer() const -> Magnum::GL::Buffer const& {
    return gl_buffer_;
}

template <typename T>
auto GLBuffer<T>::gl_buffer() -> Magnum::GL::Buffer& {
    return gl_buffer_;
}

} // namespace cuda
} // namespace ltb
