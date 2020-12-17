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
#include "ltb/util/result.hpp"

// external
#include <driver_types.h>
#include <optix_types.h>

// standard
#include <memory>

namespace ltb::cuda {

auto to_cu_device_ptr(void* ptr) -> CUdeviceptr;

struct ScopedDeviceMemory {
    explicit ScopedDeviceMemory(std::shared_ptr<void> data);

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    operator CUdeviceptr() const;

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    operator void*() const;

private:
    std::shared_ptr<void> data_;
};

auto make_scoped_device_memory(std::size_t size_in_bytes) -> util::Result<ScopedDeviceMemory>;
auto make_shared_device_memory(std::size_t size_in_bytes) -> util::Result<std::shared_ptr<void>>;

} // namespace gpu
