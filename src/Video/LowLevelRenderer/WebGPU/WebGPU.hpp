#pragma once

#if WEBGPU_BACKEND_DAWN
#include <dawn/webgpu.h>

/// @todo Remove after updating Dawn
#define WGPUMipmapFilterMode_Nearest WGPUFilterMode_Nearest
#define WGPUMipmapFilterMode_Linear WGPUFilterMode_Linear
#endif

#if WEBGPU_BACKEND_WGPU
#include <wgpu.h>

#define wgpuInstanceRelease wgpuInstanceDrop
#define wgpuSurfaceRelease wgpuSurfaceDrop
#define wgpuAdapterRelease wgpuAdapterDrop
#define wgpuDeviceRelease wgpuDeviceDrop
#define wgpuSwapChainRelease wgpuSwapChainDrop
#define wgpuBufferRelease wgpuBufferDrop
#define wgpuBindGroupRelease wgpuBindGroupDrop
#define wgpuBindGroupLayoutRelease wgpuBindGroupLayoutDrop
#define wgpuCommandEncoderRelease wgpuCommandEncoderDrop
#define wgpuCommandBufferRelease wgpuCommandBufferDrop
#define wgpuRenderPassEncoderRelease wgpuRenderPassEncoderDrop
#define wgpuComputePassEncoderRelease wgpuComputePassEncoderDrop
#define wgpuShaderModuleRelease wgpuShaderModuleDrop
#define wgpuPipelineLayoutRelease wgpuPipelineLayoutDrop
#define wgpuRenderPipelineRelease wgpuRenderPipelineDrop
#define wgpuComputePipelineRelease wgpuComputePipelineDrop
#define wgpuTextureRelease wgpuTextureDrop
#define wgpuTextureViewRelease wgpuTextureViewDrop
#define wgpuSamplerRelease wgpuSamplerDrop

inline void wgpuDeviceTick(WGPUDevice device) {
    wgpuDevicePoll(device, false, nullptr);
}
#endif