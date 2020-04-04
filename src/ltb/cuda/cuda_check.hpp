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
#include <cuda_runtime.h>

// standard
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#define CUDA_CHECK(val) ::ltb::cuda::check((val), #val, __FILE__, __LINE__)

namespace ltb {
namespace cuda {

struct CudaError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct OutOfMemoryCudaError : public CudaError {
    using CudaError::CudaError;
};

template <typename T>
void check(T result, char const* const func, const char* const file, int const line) {
    if (result != cudaSuccess) {
        std::stringstream error_str;
        error_str << "CUDA error at " << file << ":" << line;
        error_str << " code=" << static_cast<unsigned int>(result) << "(" << cudaGetErrorString(result) << ") ";
        error_str << "\"" << func << "\"";

        if (static_cast<unsigned int>(result) == 2) {
            cudaGetLastError(); // reset error state to success
            throw OutOfMemoryCudaError(error_str.str());
        } else {
            cudaDeviceReset();
            throw CudaError(error_str.str());
        }
    }
}

} // namespace cuda
} // namespace ltb
