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
#include "optix.hpp"

// project
#include "compile_ptx_from_cu_string.hpp"
#include "device_memory.h"
#include "ltb/cuda/cuda_check.hpp"
#include "ltb/cuda/optix_check.hpp"

// external
#include <cuda_runtime_api.h>
#include <optix_function_table_definition.h> // can only exist once in a translation unit
#include <optix_stack_size.h>
#include <optix_stubs.h>
#include <thrust/device_malloc.h>

// standard
#include <thread>
#include <vector>

#define DEBUG_LOGGING

#define LTB_CHECK_RESULT(val)                                                                                          \
    if (!(val)) {                                                                                                      \
        return tl::make_unexpected((val).error());                                                                     \
    }

namespace ltb::cuda {

auto OptiX::init() -> util::Result<std::shared_ptr<CUstream_st>> {
    CUstream_st* raw_stream = nullptr;
    LTB_SAFE_CUDA_CHECK(cudaStreamCreate(&raw_stream))

#ifdef DEBUG_LOGGING
    std::cerr << "stream " << raw_stream << " created" << std::endl;
    auto stream = std::shared_ptr<CUstream_st>(raw_stream, [](auto* ptr) {
        std::cerr << "stream " << ptr << " destroyed" << std::endl;
        LTB_CUDA_CHECK(cudaStreamDestroy(ptr));
    });
#else
    auto stream = std::shared_ptr<CUstream_st>(raw_stream, cudaStreamDestroy);
#endif

    {
        // Initialize CUDA
        LTB_SAFE_CUDA_CHECK(cudaFree(nullptr));

        // Initialize OptiX
        LTB_SAFE_OPTIX_CHECK(optixInit());
    }

    return stream;
}

auto OptiX::make_context(const OptixDeviceContextOptions& options)
    -> util::Result<std::shared_ptr<OptixDeviceContext_t>> {

    OptixDeviceContext_t* raw_context = nullptr;
    LTB_SAFE_OPTIX_CHECK(optixDeviceContextCreate(nullptr, &options, &raw_context))

#ifdef DEBUG_LOGGING
    std::cerr << "context " << raw_context << " created" << std::endl;
    return std::shared_ptr<OptixDeviceContext_t>(raw_context, [](auto* ptr) {
        std::cerr << "context " << ptr << " destroyed" << std::endl;
        LTB_OPTIX_CHECK(optixDeviceContextDestroy(ptr));
    });
#else
    return std::shared_ptr<OptixDeviceContext_t>(raw_context, optixDeviceContextDestroy);
#endif
}

auto OptiX::make_geometry_acceleration_structure(std::shared_ptr<OptixDeviceContext_t> const& context,
                                                 OptixAccelBuildOptions const&                accel_build_options,
                                                 OptixBuildInput const&                       build_input)
    -> util::Result<std::shared_ptr<OptixTraversableHandle>> {

#ifdef DEBUG_LOGGING
    std::cout << "[" << std::this_thread::get_id() << "]: " << __FUNCTION__ << std::endl;
#endif

    OptixAccelBufferSizes gas_buffer_sizes;
    LTB_SAFE_OPTIX_CHECK(optixAccelComputeMemoryUsage(context.get(),
                                                      &accel_build_options,
                                                      &build_input,
                                                      1, // Number of build inputs
                                                      &gas_buffer_sizes))

    auto temp_buffer_gas = make_scoped_device_memory(gas_buffer_sizes.tempSizeInBytes);
    LTB_CHECK_RESULT(temp_buffer_gas)
    auto gas_output_buffer = make_scoped_device_memory(gas_buffer_sizes.outputSizeInBytes);
    LTB_CHECK_RESULT(gas_output_buffer)

#ifdef DEBUG_LOGGING
    auto gas_handle = std::shared_ptr<OptixTraversableHandle>(new OptixTraversableHandle(0),
                                                              [context, gas_output_buffer](auto* ptr) {
                                                                  std::cerr << "gas_handle " << ptr << " destroyed"
                                                                            << std::endl;
                                                                  delete ptr;
                                                              });
    std::cerr << "gas_handle " << gas_handle.get() << " created" << std::endl;
#else
    auto gas_handle = std::shared_ptr<OptixTraversableHandle>(new OptixTraversableHandle(0),
                                                              [context, gas_output_buffer](auto* ptr) { delete ptr; });
#endif

    LTB_SAFE_OPTIX_CHECK(optixAccelBuild(context.get(),
                                         nullptr, // CUDA stream
                                         &accel_build_options,
                                         &build_input,
                                         1, // num build inputs
                                         temp_buffer_gas.value(),
                                         gas_buffer_sizes.tempSizeInBytes,
                                         gas_output_buffer.value(),
                                         gas_buffer_sizes.outputSizeInBytes,
                                         gas_handle.get(),
                                         nullptr, // emitted property list
                                         0 // num emitted properties
                                         ))

    /// \todo handle compaction like in "optixSphere" SDK sample

    return gas_handle;
}

auto OptiX::make_module(std::shared_ptr<OptixDeviceContext_t> const& context,
                        OptixPipelineCompileOptions const&           pipeline_compile_options,
                        OptixModuleCompileOptions const&             module_compile_options,
                        std::string const& ptx_str) -> util::Result<std::shared_ptr<OptixModule_t>> {

    char   log[2048];
    size_t sizeof_log = sizeof(log);

    OptixModule_t* raw_module = nullptr;
    LTB_SAFE_OPTIX_CHECK(optixModuleCreateFromPTX(context.get(),
                                                  &module_compile_options,
                                                  &pipeline_compile_options,
                                                  ptx_str.c_str(),
                                                  ptx_str.size(),
                                                  log,
                                                  &sizeof_log,
                                                  &raw_module));

#ifdef DEBUG_LOGGING
    std::cerr << "module " << raw_module << " created" << std::endl;
    return std::shared_ptr<OptixModule_t>(raw_module, [](auto* ptr) {
        std::cerr << "module " << ptr << " destroyed" << std::endl;
        LTB_OPTIX_CHECK(optixModuleDestroy(ptr));
    });
#else
    return std::shared_ptr<OptixModule_t>(raw_module, optixModuleDestroy);
#endif
}

auto OptiX::make_program_groups(std::shared_ptr<OptixDeviceContext_t> const& context,
                                std::shared_ptr<OptixModule_t> const&        module,
                                std::vector<OptixProgramGroupDesc> const&    program_group_descriptions)
    -> ltb::util::Result<std::shared_ptr<std::vector<OptixProgramGroup>>> {

#ifdef DEBUG_LOGGING
    auto program_groups = std::shared_ptr<std::vector<OptixProgramGroup>>(new std::vector<OptixProgramGroup>(),
                                                                          [context, module](auto* ptr) {
                                                                              std::cerr << "program_groups " << ptr
                                                                                        << " destroyed" << std::endl;
                                                                              delete ptr;
                                                                          });
    std::cerr << "program_groups " << program_groups.get() << " created" << std::endl;
#else
    auto program_groups = std::shared_ptr<std::vector<OptixProgramGroup>>(new std::vector<OptixProgramGroup>(),
                                                                          [context, module](auto* ptr) { delete ptr; });
#endif

    char   log[4096];
    size_t sizeof_log = sizeof(log);

    OptixProgramGroupOptions program_group_options = {}; // Initialize to zeros

    for (const auto& prog_group_desc : program_group_descriptions) {
        program_groups->emplace_back();
        auto& program_group = program_groups->back();

        LTB_SAFE_OPTIX_CHECK(optixProgramGroupCreate(context.get(),
                                                     &prog_group_desc,
                                                     1, // num program groups
                                                     &program_group_options,
                                                     log,
                                                     &sizeof_log,
                                                     &program_group));
    }

    return program_groups;
}

auto OptiX::make_pipeline(std::shared_ptr<OptixDeviceContext_t> const&           context,
                          std::shared_ptr<std::vector<OptixProgramGroup>> const& program_groups,
                          OptixPipelineCompileOptions const&                     pipeline_compile_options,
                          OptixPipelineLinkOptions const&                        pipeline_link_options)
    -> ltb::util::Result<std::shared_ptr<OptixPipeline_t>> {

    auto pipeline = std::shared_ptr<OptixPipeline_t>{};
    {
        char   log[4096];
        size_t sizeof_log = sizeof(log);

        OptixPipeline_t* raw_pipeline = nullptr;
        LTB_SAFE_OPTIX_CHECK(optixPipelineCreate(context.get(),
                                                 &pipeline_compile_options,
                                                 &pipeline_link_options,
                                                 program_groups->data(),
                                                 program_groups->size(),
                                                 log,
                                                 &sizeof_log,
                                                 &raw_pipeline));

#ifdef DEBUG_LOGGING
        std::cerr << "pipeline " << raw_pipeline << " created" << std::endl;
        pipeline = std::shared_ptr<OptixPipeline_t>(raw_pipeline, [](auto* ptr) {
            std::cerr << "pipeline " << ptr << " destroyed" << std::endl;
            LTB_OPTIX_CHECK(optixPipelineDestroy(ptr));
        });
#else
        pipeline = std::shared_ptr<OptixModule_t>(raw_pipeline, optixPipelineDestroy);
#endif
    }

    // This is optional and should be set by the application for optimal sizes.
    //    {
    //        OptixStackSizes stack_sizes = {};
    //        for (auto const& prog_group : *program_groups) {
    //            LTB_OPTIX_CHECK(optixUtilAccumulateStackSizes(prog_group, &stack_sizes));
    //        }
    //
    //        uint32_t direct_callable_stack_size_from_traversal = 0u;
    //        uint32_t direct_callable_stack_size_from_state     = 0u;
    //        uint32_t continuation_stack_size                   = 0u;
    //        LTB_OPTIX_CHECK(optixUtilComputeStackSizes(&stack_sizes,
    //                                                   pipeline_link_options.maxTraceDepth,
    //                                                   0, // maxCCDepth
    //                                                   0, // maxDCDEpth
    //                                                   &direct_callable_stack_size_from_traversal,
    //                                                   &direct_callable_stack_size_from_state,
    //                                                   &continuation_stack_size));
    //        LTB_OPTIX_CHECK(optixPipelineSetStackSize(pipeline.get(),
    //                                                  direct_callable_stack_size_from_traversal,
    //                                                  direct_callable_stack_size_from_state,
    //                                                  continuation_stack_size,
    //                                                  1 // maxTraversableDepth
    //                                                  /// \todo: ^ What should this be?
    //                                                  ));
    //    }

    return pipeline;
}

} // namespace ltb::cuda
