#pragma once

#include "../Interface/Sampler.hpp"

#include <glad/vulkan.h>

namespace Video {

class VulkanRenderer;

/// Vulkan implementation of Sampler.
class VulkanSampler : public Sampler {
  public:
    /// Create new Vulkan sampler.
    /**
     * @param device The Vulkan device.
     * @param filter The interpolation to apply.
     * @param clamping How to handle sampling outside the texture dimensions.
     */
    VulkanSampler(VkDevice device, Filter filter, Clamping clamping);

    /// Destructor.
    ~VulkanSampler() final;

    /// Get the sampler.
    /**
     * @return The sampler.
     */
    VkSampler GetSampler() const;

  private:
    VulkanSampler(const VulkanSampler& other) = delete;

    VkDevice device;
    VkSampler sampler = VK_NULL_HANDLE;
};

} // namespace Video
