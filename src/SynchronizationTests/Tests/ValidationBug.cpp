#include "ValidationBug.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/LowLevelRenderer/Vulkan/VulkanRenderer.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>

using namespace Video;

const char* ValidationBug::GetName() const {
    return "ValidationBug";
}

void ValidationBug::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R11G11B10);

    device = static_cast<VulkanRenderer*>(renderer)->GetDevice();

    // Create the event.
    VkEventCreateInfo eventInfo = {};
    eventInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;

    vkCreateEvent(device, &eventInfo, nullptr, &event);

    {
        CommandBuffer* testCmd = renderer->CreateCommandBuffer();
        VkCommandBuffer vkTestCmd = static_cast<VulkanCommandBuffer*>(testCmd)->GetCurrentCommandBuffer();

        vkCmdSetEvent(vkTestCmd, event, VK_PIPELINE_STAGE_TRANSFER_BIT);
        vkCmdWaitEvents(vkTestCmd, 1, &event, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, nullptr, 0, nullptr, 0, nullptr);

        renderer->Submit(testCmd);

        vkDeviceWaitIdle(device);

        delete testCmd;
    }

    //{
    //    CommandBuffer* testCmd = renderer->CreateCommandBuffer();
    //    VkCommandBuffer vkTestCmd = static_cast<VulkanCommandBuffer*>(testCmd)->GetCurrentCommandBuffer();
    //
    //    vkCmdResetEvent(vkTestCmd, event, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    //
    //    renderer->Submit(testCmd);
    //
    //    vkDeviceWaitIdle(device);
    //
    //    delete testCmd;
    //}

    vkResetEvent(device, event);

    {
        CommandBuffer* testCmd = renderer->CreateCommandBuffer();
        VkCommandBuffer vkTestCmd = static_cast<VulkanCommandBuffer*>(testCmd)->GetCurrentCommandBuffer();

        vkCmdSetEvent(vkTestCmd, event, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
        vkCmdWaitEvents(vkTestCmd, 1, &event, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, nullptr, 0, nullptr, 0, nullptr);

        renderer->Submit(testCmd);

        vkDeviceWaitIdle(device);

        delete testCmd;
    }


    commandBuffer = renderer->CreateCommandBuffer();
}

void ValidationBug::Frame() {
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR);
    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void ValidationBug::Shutdown() {
    vkDestroyEvent(device, event, nullptr);

    delete mainRenderTarget;
    delete commandBuffer;
}
