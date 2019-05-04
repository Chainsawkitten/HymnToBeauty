#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Video {

/// Utility functions.
namespace Utility {

/// Find the memory type to use for a certain allocation.
/**
 * @param physicalDevice The physical device.
 * @param typeFilter Filter of suitable memory types.
 * @param properties Memory properties.
 *
 * @return The memory type to use.
 */
uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

/// Create a buffer.
/**
 * @param device The Vulkan device.
 * @param physicalDevice The physical device from which to allocate memory.
 * @param size The size of the buffer.
 * @param usage How the buffer will be used.
 * @param memoryProperties Memory properties.
 * @param buffer The output buffer.
 * @param deviceMemory The output device memory.
 */
void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, unsigned int size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkBuffer* buffer, VkDeviceMemory* deviceMemory);

/// Create a 2D image, allocate and bind device memory to it.
/**
 * @param device The Vulkan device.
 * @param physicalDevice The physical device from which to allocate memory.
 * @param size The image dimensions.
 * @param mipLevels The number of mip levels in the image.
 * @param format The format of the image.
 * @param tiling Image tiling.
 * @param usage How the image will be used.
 * @param memoryProperties Memory properties.
 * @param image The output image.
 * @param deviceMemory The output device memory.
 */
void CreateImage2D(VkDevice device, VkPhysicalDevice physicalDevice, const glm::uvec2& size, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkImage* image, VkDeviceMemory* deviceMemory);

/// Record transitioning an image into a command buffer.
/**
 * @param commandBuffer The command buffer to record the transition into.
 * @param image The image to transition.
 * @param oldLayout The old layout of the image.
 * @param newLayout The new layout of the image.
 */
void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

}
}
