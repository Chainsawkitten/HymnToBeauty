#include "VulkanRenderer.hpp"

#include <Utility/Log.hpp>
#include <Utility/Time.hpp>
#include <Utility/Window.hpp>
#include <assert.h>

#include "VulkanCommandBuffer.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanBufferAllocator.hpp"
#include "VulkanVertexDescription.hpp"
#include "VulkanGeometryBinding.hpp"
#include "VulkanShader.hpp"
#include "VulkanShaderProgram.hpp"
#include "VulkanTexture.hpp"
#include "VulkanSampler.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanRenderPassAllocator.hpp"
#include "VulkanRenderTargetAllocator.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanComputePipeline.hpp"
#include "VulkanUtility.hpp"

#if ANDROID
#include <vulkan/vulkan_android.h>
#else
#include <glfw/glfw3.h>
#endif
#include <vector>
#include <cstring>
#include <cstdint>

namespace Video {

VulkanRenderer::VulkanRenderer(::Utility::Window* window) {
    optionalFeatures = {};

    CreateInstance();
    CreateSurface(window);

    SwapChainSupport swapChainSupport;
    CreateDevice(swapChainSupport);
    CreateSwapChain(swapChainSupport);
    CreateImageViews();
    CreateSyncObjects();
    CreateCommandPool();
    CreateBakedDescriptorSetLayouts();
    CreateDescriptorPool();
    if (optionalFeatures.timestamps) {
        CreateQueries();
    }
    CreateSamplers();

    for (unsigned int i = 0; i < ShaderProgram::BindingType::BINDING_TYPES; ++i) {
        currentBufferDescriptorSet[i] = 0;
    }

    for (unsigned int i = 0; i < bakedStorageBufferDescriptorSetLayouts; ++i) {
        currentStorageBufferDescriptorSet[i] = 0;
    }

    for (unsigned int i = 0; i < bakedMaterialDescriptorSetLayouts; ++i) {
        currentMaterialDescriptorSet[i] = 0;
    }

    bufferAllocator = new VulkanBufferAllocator(*this, device, physicalDevice, GetSwapChainImageCount(), nonCoherentAtomSize);
    renderPassAllocator = new VulkanRenderPassAllocator(device);
    renderTargetAllocator = new VulkanRenderTargetAllocator(*this, device, physicalDevice, renderPassAllocator);
}

VulkanRenderer::~VulkanRenderer() {
    delete bufferAllocator;
    delete renderPassAllocator;
    delete renderTargetAllocator;

    for (uint32_t i = 0; i < static_cast<uint32_t>(Sampler::Filter::COUNT) * static_cast<uint32_t>(Sampler::Clamping::COUNT); ++i) {
        delete samplers[i];
    }

    if (optionalFeatures.timestamps) {
        for (VkQueryPool queryPool : queryPools) {
            vkDestroyQueryPool(device, queryPool, nullptr);
        }
        delete[] queryCommandBuffers;

        for (std::size_t i = 0; i < swapChainImages.size(); ++i) {
            vkDestroySemaphore(device, queryResetSemaphores[i], nullptr);
        }
        delete[] queryResetSemaphores;
        delete[] needQueryWait;
        delete[] submittedTimings;
        delete[] submissionTimes;
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    for (unsigned int i = 0; i < ShaderProgram::BindingType::BINDING_TYPES; ++i) {
        vkDestroyDescriptorSetLayout(device, bufferDescriptorSetLayouts[i], nullptr);
    }
    vkDestroyDescriptorSetLayout(device, emptyDescriptorSetLayout, nullptr);
    for (unsigned int i = 0; i < bakedStorageBufferDescriptorSetLayouts; ++i) {
        vkDestroyDescriptorSetLayout(device, storageBufferDescriptorSetLayouts[i], nullptr);
    }
    for (unsigned int i = 0; i < bakedMaterialDescriptorSetLayouts; ++i) {
        vkDestroyDescriptorSetLayout(device, materialDescriptorSetLayouts[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);

    for (VkSemaphore semaphore : imageAvailableSemaphores) {
        vkDestroySemaphore(device, semaphore, nullptr);
    }

    for (VkSemaphore semaphore : renderFinishedSemaphores) {
        vkDestroySemaphore(device, semaphore, nullptr);
    }

    for (VkFence fence : renderFinishedFences) {
        vkDestroyFence(device, fence, nullptr);
    }

    vkDestroyFence(device, recreateFence, nullptr);

    FreeSwapChain();

    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

CommandBuffer* VulkanRenderer::CreateCommandBuffer() {
    return new VulkanCommandBuffer(this, device, commandPool, *renderPassAllocator);
}

void VulkanRenderer::BeginFrame() {
    bufferAllocator->BeginFrame();
    renderTargetAllocator->BeginFrame();

    AcquireSwapChainImage();
    for (unsigned int i = 0; i < ShaderProgram::BindingType::BINDING_TYPES; ++i) {
        currentBufferDescriptorSet[i] = 0;
    }

    for (unsigned int i = 0; i < bakedStorageBufferDescriptorSetLayouts; ++i) {
        currentStorageBufferDescriptorSet[i] = 0;
    }

    for (unsigned int i = 0; i < bakedMaterialDescriptorSetLayouts; ++i) {
        currentMaterialDescriptorSet[i] = 0;
    }

    // Handle queries.
    if (optionalFeatures.timestamps && currentQuery[currentFrame] > 0) {
        finishedEvents.clear();

        // Read results.
        vkGetQueryPoolResults(device, queryPools[currentFrame], 0, currentQuery[currentFrame], sizeof(uint64_t) * maxQueries, results, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

        uint64_t firstTime;
        bool firstTimeSet = false;

        for (const VulkanCommandBuffer::Timing& timing : submittedTimings[currentFrame]) {
            uint64_t start = results[timing.startQuery];
            uint64_t end = results[timing.endQuery];

            if (!firstTimeSet) {
                firstTime = start;
                firstTimeSet = true;
            }

            // Write event info.
            Profiling::Event event(timing.name);
            event.time = submissionTimes[currentFrame] + static_cast<double>(start - firstTime) * timestampPeriod / 1000000000.0;
            event.duration = static_cast<double>(end - start) * timestampPeriod / 1000000000.0;

            finishedEvents.push_back(event);
        }

        // Reset query pool.
        ResetQueries(currentFrame);

        submittedTimings[currentFrame].clear();
        currentQuery[currentFrame] = 0;
    }

    firstSubmission = true;
}

void VulkanRenderer::Submit(CommandBuffer* commandBuffer) {
    VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

    // Handle timings.
    const std::vector<VulkanCommandBuffer::Timing>& timings = vulkanCommandBuffer->GetTimings();
    for (const VulkanCommandBuffer::Timing& timing : timings) {
        submittedTimings[currentFrame].push_back(timing);
    }

    // End command buffer.
    vulkanCommandBuffer->End();

    // Synchronization.
    VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer->GetCommandBuffer();
    std::vector<VkPipelineStageFlags> waitStages;
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkSemaphore> signalSemaphores;
    VkFence fence = VK_NULL_HANDLE;

    if (optionalFeatures.timestamps && firstSubmission) {
        if (needQueryWait[currentFrame]) {
            waitStages.push_back(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
            waitSemaphores.push_back(queryResetSemaphores[currentFrame]);
            needQueryWait[currentFrame] = false;
        }

        submissionTimes[currentFrame] = ::Utility::GetTime();
    }

    if (vulkanCommandBuffer->ContainsBlitToSwapChain()) {
        waitStages.push_back(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        waitSemaphores.push_back(imageAvailableSemaphores[currentFrame]);
        signalSemaphores.push_back(renderFinishedSemaphores[currentFrame]);
        fence = renderFinishedFences[currentFrame];
    }

    // Submit command buffer to queue.
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        Log(Log::ERR) << "Could not submit command buffer to queue.\n";
    }

    vulkanCommandBuffer->NextFrame();
    firstSubmission = false;
}

void VulkanRenderer::Present() {
    // Present current frame.
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(queue, &presentInfo);
}

Buffer* VulkanRenderer::CreateBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) {
    return bufferAllocator->CreateBuffer(bufferUsage, size, data);
}

Buffer* VulkanRenderer::CreateTemporaryBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) {
    return bufferAllocator->CreateTemporaryBuffer(bufferUsage, size, data);
}

VertexDescription* VulkanRenderer::CreateVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer) {
    return new VulkanVertexDescription(attributeCount, attributes);
}

GeometryBinding* VulkanRenderer::CreateGeometryBinding(const VertexDescription* vertexDescription, Buffer* vertexBuffer, GeometryBinding::IndexType indexType, const Buffer* indexBuffer) {
    return new VulkanGeometryBinding(vertexBuffer, indexType, indexBuffer);
}

Shader* VulkanRenderer::CreateShader(const ShaderSource& shaderSource, Shader::Type type) {
    return new VulkanShader(device, shaderSource, type);
}

ShaderProgram* VulkanRenderer::CreateShaderProgram(std::initializer_list<const Shader*> shaders) {
    return new VulkanShaderProgram(this, shaders);
}

Texture* VulkanRenderer::CreateTexture(const glm::uvec2 size, Texture::Format format, int components, unsigned char* data) {
    assert(data != nullptr);

    return new VulkanTexture(*this, device, physicalDevice, size, Texture::Type::COLOR, format, components, data);
}

const Sampler* VulkanRenderer::GetSampler(Sampler::Filter filter, Sampler::Clamping clamping) const {
    assert(filter < Sampler::Filter::COUNT);
    assert(clamping < Sampler::Clamping::COUNT);

    return samplers[static_cast<uint32_t>(filter) * static_cast<uint32_t>(Sampler::Filter::COUNT) + static_cast<uint32_t>(clamping)];
}

Texture* VulkanRenderer::CreateRenderTarget(const glm::uvec2& size, Texture::Format format) {
    return renderTargetAllocator->CreateRenderTarget(size, format);
}

void VulkanRenderer::FreeRenderTarget(Texture* renderTarget) {
    renderTargetAllocator->FreeRenderTarget(renderTarget);
}

GraphicsPipeline* VulkanRenderer::CreateGraphicsPipeline(const ShaderProgram* shaderProgram, const GraphicsPipeline::Configuration& configuration, const VertexDescription* vertexDescription) {
    return new VulkanGraphicsPipeline(*this, device, shaderProgram, configuration, vertexDescription);
}

ComputePipeline* VulkanRenderer::CreateComputePipeline(const ShaderProgram* shaderProgram) {
    return new VulkanComputePipeline(device, shaderProgram);
}

void VulkanRenderer::Wait() {
    vkDeviceWaitIdle(device);
}

unsigned char* VulkanRenderer::ReadImage(Texture* texture) {
    VkImage textureImage = static_cast<VulkanTexture*>(texture)->GetImage();

    // Calculate buffer size.
    const glm::uvec2 size = texture->GetSize();
    const uint32_t bufferSize = size.x * size.y * 4 * 1;

    // Create image to map.
    VkImage image;
    VkDeviceMemory deviceMemory;
    Utility::CreateImage2D(device, physicalDevice, size, 1, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &image, &deviceMemory);

    // Create command buffer.
    CommandBuffer* commandBuffer = CreateCommandBuffer();
    VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
    VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer->GetCommandBuffer();

    // Transition source image to transfer source layout.
    Utility::TransitionImage(vkCommandBuffer, textureImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    // Transition destination image to transfer destination layout.
    Utility::TransitionImage(vkCommandBuffer, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Copy image.
    {
        VkOffset3D blitSize;
        blitSize.x = size.x;
        blitSize.y = size.y;
        blitSize.z = 1;

        VkImageBlit imageBlitRegion{};
        imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.srcSubresource.layerCount = 1;
        imageBlitRegion.srcOffsets[1] = blitSize;
        imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.dstSubresource.layerCount = 1;
        imageBlitRegion.dstOffsets[1] = blitSize;

        vkCmdBlitImage(vkCommandBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlitRegion, VK_FILTER_NEAREST);
    }

    // Transition destination image to general layout.
    Utility::TransitionImage(vkCommandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

    // Transition source image back.
    Utility::TransitionImage(vkCommandBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    Submit(commandBuffer);
    Wait();

    // Map image memory and copy data.
    unsigned char* memory;
    vkMapMemory(device, deviceMemory, 0, bufferSize, 0, reinterpret_cast<void**>(&memory));

    unsigned char* data = new unsigned char[bufferSize];
    memcpy(data, memory, bufferSize);

    vkUnmapMemory(device, deviceMemory);

    // Cleanup.
    vkDestroyImage(device, image, nullptr);
    vkFreeMemory(device, deviceMemory, nullptr);

    delete commandBuffer;

    return data;
}

const std::vector<Profiling::Event>& VulkanRenderer::GetTimeline() const {
    return finishedEvents;
}

const VulkanRenderer::OptionalFeatures& VulkanRenderer::GetOptionalFeatures() const {
    return optionalFeatures;
}

VkImage VulkanRenderer::GetCurrentSwapChainImage() const {
    return swapChainImages[imageIndex];
}

uint32_t VulkanRenderer::GetSwapChainImageCount() const {
    return swapChainImages.size();
}

VkDescriptorSetLayout VulkanRenderer::GetEmptyDescriptorSetLayout() const {
    return emptyDescriptorSetLayout;
}

VkDescriptorSetLayout VulkanRenderer::GetBufferDescriptorSetLayout(ShaderProgram::BindingType bindingType) const {
    assert(bindingType != ShaderProgram::BindingType::MATERIAL);

    return bufferDescriptorSetLayouts[bindingType];
}

VkDescriptorSetLayout VulkanRenderer::GetStorageBufferDescriptorSetLayout(unsigned int buffers) const {
    assert(buffers > 0);
    assert(buffers <= bakedStorageBufferDescriptorSetLayouts);

    return storageBufferDescriptorSetLayouts[buffers - 1];
}

VkDescriptorSetLayout VulkanRenderer::GetMaterialDescriptorSetLayout(unsigned int textures) const {
    assert(textures <= bakedMaterialDescriptorSetLayouts);

    return materialDescriptorSetLayouts[textures];
}

VkDescriptorSet VulkanRenderer::GetDescriptorSet(ShaderProgram::BindingType bindingType, VulkanBuffer* buffer) {
    assert(bindingType == ShaderProgram::BindingType::MATRICES || bindingType == ShaderProgram::BindingType::UNIFORMS);

    std::vector<VkDescriptorSet>& cache = bufferDescriptorSetCache[currentFrame][bindingType];
    VkDescriptorSetLayout descriptorSetLayout = GetBufferDescriptorSetLayout(bindingType);

    if (currentBufferDescriptorSet[bindingType] >= cache.size()) {
        // Allocate descriptor set.
        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &descriptorSetLayout;

        VkDescriptorSet descriptorSet;
        if (vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to allocate descriptor set.\n";
        }

        cache.push_back(descriptorSet);
    }

    VkDescriptorSet descriptorSet = cache[currentBufferDescriptorSet[bindingType]++];

    // Write data.
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = buffer->GetBuffer();
    descriptorBufferInfo.offset = buffer->GetOffset();
    descriptorBufferInfo.range = buffer->GetSize();

    VkDescriptorType descriptorType;
    switch (bindingType) {
    case ShaderProgram::BindingType::MATRICES:
    case ShaderProgram::BindingType::UNIFORMS:
        descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        break;
    default:
        assert(false);
    }

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.pBufferInfo = &descriptorBufferInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

    return descriptorSet;
}

VkDescriptorSet VulkanRenderer::GetStorageBufferDescriptorSet(std::initializer_list<Buffer*> buffers) {
    VkDescriptorSetLayout descriptorSetLayout = GetStorageBufferDescriptorSetLayout(buffers.size());

    std::vector<VkDescriptorSet>& cache = storageBufferDescriptorSetCache[currentFrame][descriptorSetLayout];

    if (currentStorageBufferDescriptorSet[buffers.size()] >= cache.size()) {
        // Allocate descriptor set.
        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &descriptorSetLayout;

        VkDescriptorSet descriptorSet;
        if (vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to allocate descriptor set.\n";
        }

        cache.push_back(descriptorSet);
    }

    VkDescriptorSet descriptorSet = cache[currentStorageBufferDescriptorSet[buffers.size()]++];

    // Write data.
    VkDescriptorBufferInfo* descriptorBufferInfos = new VkDescriptorBufferInfo[buffers.size()];
    unsigned int i = 0;
    for (const Buffer* buffer : buffers) {
        const VulkanBuffer* vulkanBuffer = static_cast<const VulkanBuffer*>(buffer);
        descriptorBufferInfos[i].buffer = vulkanBuffer->GetBuffer();
        descriptorBufferInfos[i].offset = vulkanBuffer->GetOffset();
        descriptorBufferInfos[i].range = vulkanBuffer->GetSize();
        ++i;
    }

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = buffers.size();
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrite.pBufferInfo = descriptorBufferInfos;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

    delete[] descriptorBufferInfos;

    return descriptorSet;
}

VkDescriptorSet VulkanRenderer::GetDescriptorSet(std::initializer_list<std::pair<Texture*, const Sampler*>> textures) {
    /// @todo Have some material object keep track of this instead of treating textures individually.

    VkDescriptorSetLayout descriptorSetLayout = GetMaterialDescriptorSetLayout(textures.size());

    std::vector<VkDescriptorSet>& cache = materialDescriptorSetCache[currentFrame][descriptorSetLayout];

    if (currentMaterialDescriptorSet[textures.size()] >= cache.size()) {
        // Allocate descriptor set.
        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &descriptorSetLayout;

        VkDescriptorSet descriptorSet;
        if (vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to allocate descriptor set.\n";
        }

        cache.push_back(descriptorSet);
    }

    VkDescriptorSet descriptorSet = cache[currentMaterialDescriptorSet[textures.size()]++];

    // Write data.
    VkDescriptorImageInfo* descriptorImageInfos = new VkDescriptorImageInfo[textures.size()];
    unsigned int i = 0;
    for (auto& texture : textures) {
        descriptorImageInfos[i].sampler = static_cast<const VulkanSampler*>(texture.second)->GetSampler();
        descriptorImageInfos[i].imageView = static_cast<const VulkanTexture*>(texture.first)->GetImageView();
        descriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        ++i;
    }

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = textures.size();
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.pImageInfo = descriptorImageInfos;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

    delete[] descriptorImageInfos;

    return descriptorSet;
}

VkQueryPool VulkanRenderer::GetQueryPool() {
    assert(optionalFeatures.timestamps);

    return queryPools[currentFrame];
}

uint32_t VulkanRenderer::GetFreeQuery() {
    assert(optionalFeatures.timestamps);

    return currentQuery[currentFrame]++;
}

void VulkanRenderer::CreateInstance() {
    // Information about our application.
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Hymn to Beauty";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "Hymn to Beauty";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_1;

    // Validation layers.
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    bool enableValidationLayers = false;
#else
    bool enableValidationLayers = true;
#endif

    if (enableValidationLayers && !CheckValidationLayersSupported(validationLayers)) {
        Log(Log::WARNING) << "Validation layers not supported.\n";
        enableValidationLayers = false;
    }

    // Get required instance extensions.
    uint32_t requiredExtensionCount; 
#if ANDROID
    requiredExtensionCount = 2u;
    const char* requiredExtensions[2] = {
        "VK_KHR_surface", "VK_KHR_android_surface"
    };
#else
    const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);
#endif

    // Create Vulkan instance.
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    if (enableValidationLayers) {
        instanceCreateInfo.enabledLayerCount = validationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    instanceCreateInfo.enabledExtensionCount = requiredExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions;

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Could not create Vulkan instance.\n";
    }
}

bool VulkanRenderer::CheckValidationLayersSupported(const std::vector<const char*>& validationLayers) const {
    // Get available layers.
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    VkLayerProperties* layerProperties = new VkLayerProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties);

    // Check if all requested layers are available.
    bool allLayersFound = true;
    for (const char* requestedLayer : validationLayers) {
        bool found = false;

        for (uint32_t layer = 0; layer < layerCount; ++layer) {
            if (strcmp(requestedLayer, layerProperties[layer].layerName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            allLayersFound = false;
            break;
        }
    }

    delete[] layerProperties;

    return allLayersFound;
}

void VulkanRenderer::CreateSurface(::Utility::Window* window) {
#if ANDROID
    VkAndroidSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.window = window->GetAndroidWindow();

    VkResult result = vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface);
#else
    VkResult result = glfwCreateWindowSurface(instance, window->GetGLFWWindow(), nullptr, &surface);
#endif

    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Could not create surface.\n";
    }
}

void VulkanRenderer::CreateDevice(SwapChainSupport& swapChainSupport) {
    // The device extensions we require.
    const std::vector<const char*> mandatoryDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Pick the best physical device.
    physicalDevice = PickPhysicalDevice(mandatoryDeviceExtensions);
    if (physicalDevice == VK_NULL_HANDLE) {
        Log(Log::ERR) << "Could not find suitable device.\n";
    }
    swapChainSupport = GetSwapChainSupport(physicalDevice);

    // Create logical device.
    const float queuePriority = 1.0f;
    graphicsQueueFamily = GetQueueFamily(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    const std::vector<const char*> deviceExtensions = GetEnabledExtensions(mandatoryDeviceExtensions);
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkPhysicalDeviceFeatures physicalDeviceFeatures = GetEnabledFeatures();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create device.\n";
    }

    // Get the queue to submit work to.
    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &queue);
}

VkPhysicalDevice VulkanRenderer::PickPhysicalDevice(const std::vector<const char*>& extensions) const {
    // Get all physical devices.
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices);

    Log(Log::INFO) << "Available devices:\n";

    // Find which is best.
    uint32_t bestDeviceScore = 0;
    uint32_t bestDevice = 0;
    for (uint32_t device = 0; device < deviceCount; ++device) {
        SwapChainSupport support;
        uint32_t score = GetDeviceScore(physicalDevices[device], extensions);

        if (score > bestDeviceScore) {
            bestDevice = device;
            bestDeviceScore = score;
        }
    }

    VkPhysicalDevice selectedDevice = physicalDevices[bestDevice];

    delete[] physicalDevices;

    if (bestDeviceScore == 0) {
        Log(Log::ERR) << "No suitable device found.\n";
        return VK_NULL_HANDLE;
    }

    Log(Log::INFO) << "Selected device " << bestDevice << ".\n";

    return selectedDevice;
}

uint32_t VulkanRenderer::GetDeviceScore(VkPhysicalDevice device, const std::vector<const char*>& extensions) const {
    // Get device properties.
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    Log(Log::INFO) << " - " << deviceProperties.deviceName << "\n";

    /* First determine if device supports all required functionality.
       Otherwise return a score of 0. */

    // Check that all required features are supported.
    if (!deviceFeatures.fragmentStoresAndAtomics || !deviceFeatures.depthClamp) {
        return 0;
    }

    // Check that the required extensions are supported.
    if (!CheckDeviceExtensionsSupported(device, extensions))
        return 0;

    // Check that we have a supported queue family.
    uint32_t queueFamily = GetQueueFamily(device);
    if (queueFamily == std::numeric_limits<uint32_t>::max())
        return 0;

    // Score device.
    uint32_t score = 1;

    // Prefer a dedicated graphics card.
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    return score;
}

bool VulkanRenderer::CheckDeviceExtensionsSupported(VkPhysicalDevice device, const std::vector<const char*>& extensions) const {
    // Get available extensions.
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    VkExtensionProperties* extensionProperties = new VkExtensionProperties[extensionCount];
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensionProperties);

    // Check if all requested extensions are supported.
    bool allExtensionsFound = true;
    for (const char* requestedExtension : extensions) {
        bool found = false;

        for (uint32_t extension = 0; extension < extensionCount; ++extension) {
            if (strcmp(requestedExtension, extensionProperties[extension].extensionName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            allExtensionsFound = false;
            break;
        }
    }

    delete[] extensionProperties;

    return allExtensionsFound;
}

uint32_t VulkanRenderer::GetQueueFamily(VkPhysicalDevice device) const {
    // Get all queue families.
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties);

    // Find queue family that supports graphics, compute and presentation.
    uint32_t queueFamily = std::numeric_limits<uint32_t>::max();
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilyProperties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
            VkBool32 presentationSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupported);
            if (presentationSupported) {
                queueFamily = i;
                break;
            }
        }
    }

    delete[] queueFamilyProperties;

    return queueFamily;
}

VulkanRenderer::SwapChainSupport VulkanRenderer::GetSwapChainSupport(VkPhysicalDevice device) const {
    SwapChainSupport support;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);

    // Get supported formats.
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount > 0) {
        support.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, support.formats.data());
    }

    // Get supported present modes.
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount > 0) {
        support.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, support.presentModes.data());
    }

    return support;
}

const std::vector<const char*> VulkanRenderer::GetEnabledExtensions(const std::vector<const char*>& mandatoryExtensions) {
    std::vector<const char*> extensions;

    // Enable mandatory extensions.
    for (const char* extension : mandatoryExtensions) {
        extensions.push_back(extension);
    }

    // Get available extensions.
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    VkExtensionProperties* extensionProperties = new VkExtensionProperties[extensionCount];
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties);

    // Check if optional extensions are supported.
    for (uint32_t extension = 0; extension < extensionCount; ++extension) {
        // Conservative rasterization.
        if (strcmp(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, extensionProperties[extension].extensionName) == 0) {
            extensions.push_back(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME);
            optionalFeatures.conservativeRasterization = true;
        }
    }

    delete[] extensionProperties;

    return extensions;
}

VkPhysicalDeviceFeatures VulkanRenderer::GetEnabledFeatures() {
    // Get all supported features.
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    // Enabled mandatory features.
    VkPhysicalDeviceFeatures enabledFeatures = {};
    enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
    enabledFeatures.depthClamp = VK_TRUE;

    // Enable optional features.
    optionalFeatures.fillModeNonSolid = supportedFeatures.fillModeNonSolid;
    enabledFeatures.fillModeNonSolid = supportedFeatures.fillModeNonSolid;
    optionalFeatures.wideLines = supportedFeatures.wideLines;
    enabledFeatures.wideLines = supportedFeatures.wideLines;

    // Check limits.
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    optionalFeatures.timestamps = deviceProperties.limits.timestampComputeAndGraphics;
    if (!optionalFeatures.timestamps) {
        Log(Log::WARNING) << "timestampComputeAndGraphics not supported on the device. GPU profiling will not be supported.\n";
    }
    timestampPeriod = deviceProperties.limits.timestampPeriod;
    nonCoherentAtomSize = static_cast<uint32_t>(deviceProperties.limits.nonCoherentAtomSize);

    optionalFeatures.attachmentlessMsaaSamples = deviceProperties.limits.framebufferNoAttachmentsSampleCounts;

    return enabledFeatures;
}

void VulkanRenderer::CreateSwapChain(const SwapChainSupport& swapChainSupport) {
    // Determine what settings to use for the swap chain.
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapChainSupport);
    swapChainImageFormat = surfaceFormat.format;
    VkPresentModeKHR presentMode = ChoosePresentMode(swapChainSupport);
    swapChainExtent = ChooseExtent(swapChainSupport);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Create swap chain.
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = swapChainExtent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    /// @todo Sharing mode if graphicsFamily != presentFamily
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;

    VkResult result = vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create swap chain.\n";
    }

    // Retrieve swap chain images.
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
}

void VulkanRenderer::FreeSwapChain() {
    for (VkImageView imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void VulkanRenderer::RecreateSwapChain() {
    Wait();

    FreeSwapChain();

    SwapChainSupport swapChainSupport = GetSwapChainSupport(physicalDevice);
    CreateSwapChain(swapChainSupport);
    CreateImageViews();
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSurfaceFormat(const SwapChainSupport& swapChainSupport) const {
    for (const VkSurfaceFormatKHR& format : swapChainSupport.formats) {
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_R8G8B8A8_UNORM) {
            return format;
        }
    }

    return swapChainSupport.formats[0];
}

VkPresentModeKHR VulkanRenderer::ChoosePresentMode(const SwapChainSupport& swapChainSupport) const {
    for (const VkPresentModeKHR& presentMode : swapChainSupport.presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::ChooseExtent(const SwapChainSupport& swapChainSupport) const {
    VkExtent2D extent = swapChainSupport.capabilities.currentExtent;

    /// @todo Properly determine.

    return extent;
}

void VulkanRenderer::CreateImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); ++i) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
        if (result != VK_SUCCESS) {
            Log(Log::ERR) << "Could not create image view.\n";
        }
    }
}

void VulkanRenderer::CreateSyncObjects() {
    imageAvailableSemaphores.resize(swapChainImages.size());
    renderFinishedSemaphores.resize(swapChainImages.size());
    renderFinishedFences.resize(swapChainImages.size());

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (std::size_t i = 0; i < swapChainImages.size(); ++i) {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS) {
            Log(Log::ERR) << "Could not create semaphore.\n";
        }

        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
            Log(Log::ERR) << "Could not create semaphore.\n";
        }

        if (vkCreateFence(device, &fenceCreateInfo, nullptr, &renderFinishedFences[i]) != VK_SUCCESS) {
            Log(Log::ERR) << "Could not create fence.\n";
        }
    }

    if (vkCreateFence(device, &fenceCreateInfo, nullptr, &recreateFence) != VK_SUCCESS) {
        Log(Log::ERR) << "Could not create fence.\n";
    }
}

void VulkanRenderer::CreateCommandPool() {
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamily;

    VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Could not create command pool.\n";
    }
}

void VulkanRenderer::CreateBakedDescriptorSetLayouts() {
    // Clear buffer descriptor set layouts.
    for (unsigned int i = 0; i < ShaderProgram::BindingType::BINDING_TYPES; ++i) {
        bufferDescriptorSetLayouts[i] = VK_NULL_HANDLE;
    }

    // Create buffer descriptor set layouts.
    VkDescriptorSetLayout descriptorSetLayout;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = 1;

    // Uniform buffers.
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;

        layoutCreateInfo.pBindings = &uboLayoutBinding;

        // MATRICES
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        
        if (vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to create descriptor set layout.\n";
        }

        bufferDescriptorSetLayouts[ShaderProgram::BindingType::MATRICES] = descriptorSetLayout;

        // UNIFORMS
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
        
        if (vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to create descriptor set layout.\n";
        }

        bufferDescriptorSetLayouts[ShaderProgram::BindingType::UNIFORMS] = descriptorSetLayout;
    }

    bufferDescriptorSetCache.resize(swapChainImages.size());

    // Storage buffers.
    VkDescriptorSetLayoutBinding storageBufferLayoutBindings[bakedStorageBufferDescriptorSetLayouts];
    layoutCreateInfo.pBindings = storageBufferLayoutBindings;
    for (unsigned int i = 0; i < bakedStorageBufferDescriptorSetLayouts; ++i) {
        storageBufferLayoutBindings[i].binding = i;
        storageBufferLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        storageBufferLayoutBindings[i].descriptorCount = 1;
        storageBufferLayoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        layoutCreateInfo.bindingCount = i + 1;

        if (vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to create descriptor set layout.\n";
        }

        storageBufferDescriptorSetLayouts[i] = descriptorSetLayout;
    }

    storageBufferDescriptorSetCache.resize(swapChainImages.size());

    // Empty descriptor set layout.
    layoutCreateInfo.bindingCount = 0;

    if (vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create descriptor set layout.\n";
    }

    emptyDescriptorSetLayout = descriptorSetLayout;

    // Material descriptor set layouts.
    VkDescriptorSetLayoutBinding materialLayoutBindings[bakedMaterialDescriptorSetLayouts];
    layoutCreateInfo.pBindings = materialLayoutBindings;
    for (unsigned int i = 0; i < bakedMaterialDescriptorSetLayouts; ++i) {
        materialLayoutBindings[i].binding = i;
        materialLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialLayoutBindings[i].descriptorCount = 1;
        materialLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        materialLayoutBindings[i].pImmutableSamplers = nullptr;

        layoutCreateInfo.bindingCount = i + 1;

        if (vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to create descriptor set layout.\n";
        }

        materialDescriptorSetLayouts[i] = descriptorSetLayout;
    }

    materialDescriptorSetCache.resize(swapChainImages.size());
}

void VulkanRenderer::CreateDescriptorPool() {
    const uint32_t maxSets = 1000;

    std::vector<VkDescriptorPoolSize> poolSizes;

    // Uniform buffers.
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = maxSets;
        poolSizes.push_back(poolSize);
    }

    // Storage buffers.
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSize.descriptorCount = maxSets;
        poolSizes.push_back(poolSize);
    }

    // Combined image samplers.
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = maxSets * 3;
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = 0;
    poolCreateInfo.maxSets = maxSets;
    poolCreateInfo.poolSizeCount = poolSizes.size();
    poolCreateInfo.pPoolSizes = poolSizes.data();

    if (vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        Log(Log::ERR) << "Could not create descriptor pool.\n";
    }
}

void VulkanRenderer::CreateQueries() {
    assert(optionalFeatures.timestamps);

    // Create command buffers for resetting query pools.
    queryCommandBuffers = new VkCommandBuffer[swapChainImages.size()];

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = swapChainImages.size();

    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, queryCommandBuffers) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create command buffers for resetting queries.\n";
    }

    // Create semaphores for resetting query pools.
    queryResetSemaphores = new VkSemaphore[swapChainImages.size()];
    needQueryWait = new bool[swapChainImages.size()];

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (std::size_t i = 0; i < swapChainImages.size(); i++) {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &queryResetSemaphores[i]) != VK_SUCCESS) {
            Log(Log::ERR) << "Could not create semaphore.\n";
        }

        needQueryWait[i] = false;
    }

    // Create query pools.
    VkQueryPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    poolCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    poolCreateInfo.queryCount = maxQueries;

    for (std::size_t i = 0; i < swapChainImages.size(); i++) {
        VkQueryPool queryPool;
        if (vkCreateQueryPool(device, &poolCreateInfo, nullptr, &queryPool) != VK_SUCCESS) {
            Log(Log::ERR) << "Could not create query pool.\n";
        }
        queryPools.push_back(queryPool);

        currentQuery.push_back(0);

        ResetQueries(i);
    }

    submittedTimings = new std::vector<VulkanCommandBuffer::Timing>[swapChainImages.size()];
    submissionTimes = new double[swapChainImages.size()];
}

void VulkanRenderer::ResetQueries(uint32_t queryPool) {
    assert(optionalFeatures.timestamps);

    vkResetCommandBuffer(queryCommandBuffers[queryPool], 0);

    // Begin command buffer.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(queryCommandBuffers[queryPool], &beginInfo) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to begin command buffer.\n";
    }

    // Reset queries.
    vkCmdResetQueryPool(queryCommandBuffers[queryPool], queryPools[queryPool], 0, maxQueries);

    // Submit.
    vkEndCommandBuffer(queryCommandBuffers[queryPool]);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &queryCommandBuffers[queryPool];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &queryResetSemaphores[queryPool];

    if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        Log(Log::ERR) << "Could not submit command buffer to queue.\n";
    }

    needQueryWait[queryPool] = true;
}

void VulkanRenderer::CreateSamplers() {
    for (uint32_t i = 0; i < static_cast<uint32_t>(Sampler::Filter::COUNT) * static_cast<uint32_t>(Sampler::Clamping::COUNT); ++i) {
        samplers[i] = new VulkanSampler(device,
                                        static_cast<Sampler::Filter>(i / static_cast<uint32_t>(Sampler::Filter::COUNT)),
                                        static_cast<Sampler::Clamping>(i % static_cast<uint32_t>(Sampler::Filter::COUNT)));
    }
}

void VulkanRenderer::AcquireSwapChainImage() {
    currentFrame = (currentFrame + 1) % swapChainImages.size();

    // Wait for previous rendering to this swap chain image to complete.
    vkWaitForFences(device, 1, &renderFinishedFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &renderFinishedFences[currentFrame]);

    // Acquire next image.
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swap chain has changed, recreate it.
        Log(Log::INFO) << "Recreating swap chain.\n";
        RecreateSwapChain();

        VkSemaphore semaphore = VK_NULL_HANDLE;
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // Semaphore will never be signaled by previous vkAcquireNextImageKHR, we need to signal it again.
            semaphore = imageAvailableSemaphores[currentFrame];
        }

        vkResetFences(device, 1, &recreateFence);
        vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, semaphore, recreateFence, &imageIndex);
        vkWaitForFences(device, 1, &recreateFence, VK_TRUE, UINT64_MAX);
    } else if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to acquire swap chain image.\n";
    }

    // Wait for next frame to be finished so we don't overwrite in-flight resources.
    vkWaitForFences(device, 1, &renderFinishedFences[(currentFrame + 1) % swapChainImages.size()], VK_TRUE, UINT64_MAX);
}

} // namespace Video
