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
#include "device_memory.h"

// project
#include "ltb/cuda/cuda_check.hpp"

// external
#include <cuda_runtime.h>

namespace ltb::cuda {

auto to_cu_device_ptr(void* ptr) -> CUdeviceptr {
    return reinterpret_cast<CUdeviceptr>(ptr);
}

ScopedDeviceMemory::ScopedDeviceMemory(std::shared_ptr<void> data) : data_(std::move(data)) {}

ScopedDeviceMemory::operator CUdeviceptr() const {
    return to_cu_device_ptr(data_.get());
}

ScopedDeviceMemory::operator void*() const {
    return data_.get();
}

auto make_scoped_device_memory(std::size_t size_in_bytes) -> util::Result<ScopedDeviceMemory> {
    auto shared_device_memory = make_shared_device_memory(size_in_bytes);
    if (shared_device_memory) {
        return ScopedDeviceMemory(std::move(shared_device_memory.value()));
    } else {
        return tl::make_unexpected(std::move(shared_device_memory.error()));
    }
}

auto make_shared_device_memory(std::size_t size_in_bytes) -> util::Result<std::shared_ptr<void>> {
    void* ptr = nullptr;
    LTB_SAFE_CUDA_CHECK(cudaMalloc(&ptr, size_in_bytes))
    return std::shared_ptr<void>(ptr, [](auto* _ptr) { LTB_CUDA_CHECK(cudaFree(_ptr)); });
}

} // namespace ltb::cuda
