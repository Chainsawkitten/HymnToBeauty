#include "VulkanTexture.hpp"

#include <algorithm>
#include <cmath>
#include <Utility/Log.hpp>
#include "VulkanUtility.hpp"
#include "VulkanRenderer.hpp"
#include "VulkanCommandBuffer.hpp"

namespace Video {

VulkanTexture::VulkanTexture(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, const glm::uvec2 size, Texture::Type type, Texture::Format format, int components, unsigned char* data) : Texture(type, size) {
    assert(size.x > 0 && size.y > 0);
    assert((data != nullptr && components >= 1 && components <= 4) || (data == nullptr && components == 0));
    assert((type == Texture::Type::COLOR && data != nullptr) || (type != Texture::Type::COLOR && data == nullptr));

    this->device = device;

    // Get Vulkan internal format.
    VkImageUsageFlags usage;
    VkImageAspectFlags aspectMask;
    switch (type) {
    case Texture::Type::COLOR:
        usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;
    case Texture::Type::RENDER_COLOR:
        usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        break;
    case Texture::Type::RENDER_DEPTH:
        usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        break;
    default:
        assert(false);
    }

    switch (format) {
    case Texture::Format::R8G8B8A8:
        assert(type != Texture::Type::RENDER_DEPTH);
        internalFormat = VK_FORMAT_R8G8B8A8_UNORM;
        break;
    case Texture::Format::R11G11B10:
        assert(type != Texture::Type::RENDER_DEPTH);
        internalFormat = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        break;
    case Texture::Format::D32:
        assert(type == Texture::Type::RENDER_DEPTH);
        internalFormat = VK_FORMAT_D32_SFLOAT;
        break;
    default:
        assert(false);
    }

    // Calculate the number of mip levels.
    const uint32_t mipLevels = (type == Texture::Type::COLOR ? static_cast<uint32_t>(std::log2(std::max(size.x, size.y))) + 1 : 1);

    // Create image.
    Utility::CreateImage2D(device, physicalDevice, size, mipLevels, internalFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &deviceMemory);

    // Create image view.
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = internalFormat;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create image view.\n";
    }

    if (type == Texture::Type::COLOR) {
        /// @todo Support textures of different formats.
        assert(internalFormat == VK_FORMAT_R8G8B8A8_UNORM);

        // Create staging buffer.
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingDeviceMemory;
        uint32_t bufferSize = size.x * size.y * 4;
        Utility::CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingDeviceMemory);

        // Copy data to staging buffer.
        char* stagingData;
        vkMapMemory(device, stagingDeviceMemory, 0, bufferSize, 0, reinterpret_cast<void**>(&stagingData));

        for (uint32_t i = 0; i < size.x * size.y; ++i) {
            stagingData[i * 4] = data[i * components];
            stagingData[i * 4 + 1] = (components > 1 ? data[i * components + 1] : 0);
            stagingData[i * 4 + 2] = (components > 1 ? data[i * components + 2] : 0);
            stagingData[i * 4 + 3] = (components > 1 ? data[i * components + 3] : 0);
        }

        vkUnmapMemory(device, stagingDeviceMemory);

        // Copy from staging buffer to image.
        CommandBuffer* commandBuffer = vulkanRenderer.CreateCommandBuffer();
        VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

        Utility::TransitionImage(vulkanCommandBuffer->GetCommandBuffer(), image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy bufferImageCopy = {};
        bufferImageCopy.bufferOffset = 0;
        bufferImageCopy.bufferRowLength = 0;
        bufferImageCopy.bufferImageHeight = 0;
        bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferImageCopy.imageSubresource.mipLevel = 0;
        bufferImageCopy.imageSubresource.baseArrayLayer = 0;
        bufferImageCopy.imageSubresource.layerCount = 1;
        bufferImageCopy.imageOffset.x = 0;
        bufferImageCopy.imageOffset.y = 0;
        bufferImageCopy.imageOffset.z = 0;
        bufferImageCopy.imageExtent.width = size.x;
        bufferImageCopy.imageExtent.height = size.y;
        bufferImageCopy.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(vulkanCommandBuffer->GetCommandBuffer(), stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

        vulkanRenderer.Submit(commandBuffer);
        vulkanRenderer.Wait();

        // Cleanup.
        delete commandBuffer;
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingDeviceMemory, nullptr);

        GenerateMipMaps(vulkanRenderer, physicalDevice, size, mipLevels, internalFormat);
    }

    if (type != Texture::Type::RENDER_DEPTH) {
        /// @todo These could be cached somewhere since they're not image specific.
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 1000.0f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        /// @todo Configurable filtering.
        switch (type) {
        case Texture::Type::COLOR:
            samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
            samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
            samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            break;
        case Texture::Type::RENDER_COLOR:
            samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
            samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
            samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
        case Texture::Type::RENDER_DEPTH:
            samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
            samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
            samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
        }

        if (vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to create sampler.\n";
        }
    }

    if (type == Texture::Type::RENDER_COLOR || type == Texture::Type::RENDER_DEPTH) {
        // Transition to attachment layout.
        currentLayout = (type == Texture::Type::RENDER_DEPTH ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        CommandBuffer* commandBuffer = vulkanRenderer.CreateCommandBuffer();
        VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

        Utility::TransitionImage(vulkanCommandBuffer->GetCommandBuffer(), image, VK_IMAGE_LAYOUT_UNDEFINED, currentLayout);

        vulkanRenderer.Submit(commandBuffer);
        vulkanRenderer.Wait();

        delete commandBuffer;
    }
}

VulkanTexture::~VulkanTexture() {
    vkDestroySampler(device, sampler, nullptr);
    vkDestroyImageView(device, imageView, nullptr);
    vkDestroyImage(device, image, nullptr);
    vkFreeMemory(device, deviceMemory, nullptr);
}

VkFormat VulkanTexture::GetFormat() const {
    return internalFormat;
}

VkImage VulkanTexture::GetImage() const {
    return image;
}

VkImageView VulkanTexture::GetImageView() const {
    return imageView;
}

VkSampler VulkanTexture::GetSampler() const {
    return sampler;
}

VkImageLayout VulkanTexture::GetImageLayout() const {
    return currentLayout;
}

void VulkanTexture::SetImageLayout(VkImageLayout layout) {
    currentLayout = layout;
}

void VulkanTexture::GenerateMipMaps(VulkanRenderer& vulkanRenderer, VkPhysicalDevice physicalDevice, const glm::uvec2& size, uint32_t mipLevels, VkFormat format) {
#ifndef NDEBUG
    // Check that the image format supports linear blitting.
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);
#endif

    CommandBuffer* commandBuffer = vulkanRenderer.CreateCommandBuffer();
    VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    // Generate mipmaps.
    int32_t mipWidth = size.x;
    int32_t mipHeight = size.y;
    for (uint32_t i = 1; i < mipLevels; ++i) {
        // Transition src mip level to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL.
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(vulkanCommandBuffer->GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        // Blit mip level to lower one.
        VkImageBlit blit = {};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(vulkanCommandBuffer->GetCommandBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        // Transition src mip level to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(vulkanCommandBuffer->GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    // Transition final mip level to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(vulkanCommandBuffer->GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    vulkanRenderer.Submit(commandBuffer);
    vulkanRenderer.Wait();

    // Cleanup.
    delete commandBuffer;
}

}
