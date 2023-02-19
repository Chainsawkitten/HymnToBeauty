#include "WebGPUSampler.hpp"

#include <cassert>

namespace Video {

WebGPUSampler::WebGPUSampler(WGPUDevice device, Filter filter, Clamping clamping) {
    assert(filter < Filter::COUNT);
    assert(clamping < Clamping::COUNT);

    WGPUSamplerDescriptor samplerDescriptor = {};
    samplerDescriptor.maxAnisotropy = 1;
    samplerDescriptor.compare = WGPUCompareFunction_Undefined;
    samplerDescriptor.lodMinClamp = 0.0f;
    samplerDescriptor.lodMaxClamp = 1000.0f;

    switch (filter) {
    case Filter::NEAREST:
        samplerDescriptor.magFilter = WGPUFilterMode_Nearest;
        samplerDescriptor.minFilter = WGPUFilterMode_Nearest;
        samplerDescriptor.mipmapFilter = WGPUFilterMode_Nearest;
        break;
    case Filter::LINEAR:
        samplerDescriptor.magFilter = WGPUFilterMode_Linear;
        samplerDescriptor.minFilter = WGPUFilterMode_Linear;
        samplerDescriptor.mipmapFilter = WGPUFilterMode_Linear;
        break;
    default:
        assert(false);
    }

    switch (clamping) {
    case Clamping::REPEAT:
        samplerDescriptor.addressModeU = WGPUAddressMode_Repeat;
        samplerDescriptor.addressModeV = WGPUAddressMode_Repeat;
        samplerDescriptor.addressModeW = WGPUAddressMode_Repeat;
        break;
    case Clamping::CLAMP_TO_EDGE:
        samplerDescriptor.addressModeU = WGPUAddressMode_ClampToEdge;
        samplerDescriptor.addressModeV = WGPUAddressMode_ClampToEdge;
        samplerDescriptor.addressModeW = WGPUAddressMode_ClampToEdge;
        break;
    default:
        assert(false);
    }

    sampler = wgpuDeviceCreateSampler(device, &samplerDescriptor);
}

WebGPUSampler::~WebGPUSampler() {
    wgpuSamplerRelease(sampler);
}

WGPUSampler WebGPUSampler::GetSampler() const {
    return sampler;
}

}
