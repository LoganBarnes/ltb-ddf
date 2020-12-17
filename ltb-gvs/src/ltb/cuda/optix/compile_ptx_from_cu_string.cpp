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
#include "compile_ptx_from_cu_string.hpp"

// project
#include "ltb/gvs_cuda_includes.hpp"

// external
#include <nvrtc.h>

// standard
#include <sstream>

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
#define LINE_STR STRINGIFY(__LINE__)

#define LTB_SAFE_NVRTC_CHECK(val)                                                                                      \
    {                                                                                                                  \
        auto error_message = nvrtc_error_string((val), #val, __FILE__, __LINE__);                                      \
        if (!error_message.empty()) {                                                                                  \
            return tl::make_unexpected(LTB_MAKE_ERROR(error_message));                                                 \
        }                                                                                                              \
    }

namespace ltb::cuda {
namespace {

template <typename T>
auto nvrtc_error_string(T result, char const* const func, const char* const file, int const line) -> std::string {
    if (result != NVRTC_SUCCESS) {
        std::stringstream error_str;
        error_str << "NVRTC error at " << file << ":" << line << "\n";
        error_str << "\tcode=" << static_cast<unsigned int>(result) << "(" << nvrtcGetErrorString(result) << ")\n";
        error_str << "\t\"" << func << "\"";
        return error_str.str();
    }
    return "";
}

} // namespace

auto compile_ptx_from_cu_string(const std::string&       cu_source,
                                const std::string&       name,
                                std::vector<std::string> include_dirs,
                                std::vector<std::string> compiler_flags,
                                const char**             log_string) -> ltb::util::Result<std::string> {

    // Create program
    nvrtcProgram prog = nullptr;
    LTB_SAFE_NVRTC_CHECK(nvrtcCreateProgram(&prog, cu_source.c_str(), name.c_str(), 0, nullptr, nullptr));

    include_dirs.emplace_back(paths::cuda_include_dir());
    include_dirs.emplace_back(paths::optix_include_dir());
    include_dirs.emplace_back(paths::gvs_root() + "external");

    auto architectures = paths::cuda_architectures();

    /// \todo: Check for these flags before setting them so they aren't set them twice.

    // If multiple architectures are specified then we leave it to the user to specify the correct flags.
    if (architectures.size() == 1) {
        compiler_flags.emplace_back("-arch");
        compiler_flags.emplace_back("compute_" + std::to_string(architectures.front()));
    }
    compiler_flags.emplace_back("-rdc");
    compiler_flags.emplace_back("true");
    compiler_flags.emplace_back("-D__x86_64");

    // Gather NVRTC options
    auto include_dir_options = std::vector<std::string>{};

    // Collect include dirs
    for (const std::string& dir : include_dirs) {
        include_dir_options.push_back("-I" + dir);
    }

    // Collect NVRTC options
    auto options = std::vector<const char*>{};

    for (const auto& flag : compiler_flags) {
        options.emplace_back(flag.c_str());
    }
    for (const auto& option : include_dir_options) {
        options.emplace_back(option.c_str());
    }

    // JIT compile CU to PTX
    const nvrtcResult compile_res = nvrtcCompileProgram(prog, static_cast<int>(options.size()), options.data());

    // Retrieve log output
    size_t log_size = 0;
    LTB_SAFE_NVRTC_CHECK(nvrtcGetProgramLogSize(prog, &log_size));

    std::string g_nvrtc_log;
    g_nvrtc_log.resize(log_size);
    if (log_size > 1) {
        LTB_SAFE_NVRTC_CHECK(nvrtcGetProgramLog(prog, &g_nvrtc_log[0]));
        if (log_string) {
            *log_string = g_nvrtc_log.c_str();
        }
    }

    if (compile_res != NVRTC_SUCCESS) {
        return tl::make_unexpected(LTB_MAKE_ERROR("NVRTC Compilation failed.\n" + g_nvrtc_log));
    }

    // Retrieve PTX code
    std::string ptx;
    size_t      ptx_size = 0;
    LTB_SAFE_NVRTC_CHECK(nvrtcGetPTXSize(prog, &ptx_size));
    ptx.resize(ptx_size);
    LTB_SAFE_NVRTC_CHECK(nvrtcGetPTX(prog, &ptx[0]));

    // Cleanup
    LTB_SAFE_NVRTC_CHECK(nvrtcDestroyProgram(&prog));
    return ptx;
}

} // namespace ltb::cuda
