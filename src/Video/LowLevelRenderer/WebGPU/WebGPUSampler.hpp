#pragma once

#include "../Interface/Sampler.hpp"

#include <dawn/webgpu.h>

namespace Video {

/// WebGPU implementation of Sampler.
class WebGPUSampler : public Sampler {
  public:
    /// Create new WebGPU sampler.
    /**
     * @param device The WebGPU device.
     * @param filter The interpolation to apply.
     * @param clamping How to handle sampling outside the texture dimensions.
     */
    WebGPUSampler(WGPUDevice device, Filter filter, Clamping clamping);

    /// Destructor.
    ~WebGPUSampler() final;

    /// Get the sampler.
    /**
     * @return The sampler.
     */
    WGPUSampler GetSampler() const;

  private:
    WebGPUSampler(const WebGPUSampler& other) = delete;

    WGPUSampler sampler;
};

} // namespace Video
