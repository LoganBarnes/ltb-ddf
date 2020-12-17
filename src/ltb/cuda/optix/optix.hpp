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

class OptiX {
public:
    /// \brief
    /// \return
    static auto init() -> util::Result<std::shared_ptr<CUstream_st>>;

    /// \brief
    /// \param options
    /// \return
    static auto make_context(OptixDeviceContextOptions const& options)
        -> util::Result<std::shared_ptr<OptixDeviceContext_t>>;

    /// \brief
    /// \param context
    /// \param accel_build_options
    /// \return
    static auto make_geometry_acceleration_structure(std::shared_ptr<OptixDeviceContext_t> const& context,
                                                     OptixAccelBuildOptions const&                accel_build_options,
                                                     OptixBuildInput const&                       build_input)
        -> util::Result<std::shared_ptr<OptixTraversableHandle>>;

    /// \brief
    /// \param context
    /// \param pipeline_compile_options
    /// \param module_compile_options
    /// \param ptx_str
    /// \return
    static auto make_module(std::shared_ptr<OptixDeviceContext_t> const& context,
                            OptixPipelineCompileOptions const&           pipeline_compile_options,
                            OptixModuleCompileOptions const&             module_compile_options,
                            std::string const& ptx_str) -> util::Result<std::shared_ptr<OptixModule_t>>;

    /// \brief
    /// \param context
    /// \param module
    /// \param program_group_descriptions
    /// \return
    static auto make_program_groups(std::shared_ptr<OptixDeviceContext_t> const& context,
                                    std::shared_ptr<OptixModule_t> const&        module,
                                    std::vector<OptixProgramGroupDesc> const&    program_group_descriptions)
        -> ltb::util::Result<std::shared_ptr<std::vector<OptixProgramGroup>>>;

    /// \brief
    /// \param context
    /// \param program_groups
    /// \param pipeline_compile_options
    /// \param pipeline_link_options
    /// \return
    static auto make_pipeline(std::shared_ptr<OptixDeviceContext_t> const&           context,
                              std::shared_ptr<std::vector<OptixProgramGroup>> const& program_groups,
                              OptixPipelineCompileOptions const&                     pipeline_compile_options,
                              OptixPipelineLinkOptions const&                        pipeline_link_options)
        -> ltb::util::Result<std::shared_ptr<OptixPipeline_t>>;
};

} // namespace ltb::cuda
