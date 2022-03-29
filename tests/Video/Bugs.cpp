#include "VideoSuite.hpp"

#include "ImageVerification.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/RenderPass.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/LowLevelRenderer/Interface/ComputePipeline.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/Texture/Texture2D.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "FullscreenTriangle.vert.hpp"
#include "VerifyFillBuffer.frag.hpp"
#include "WriteBuffer.comp.hpp"

#include "BugFillBuffer.png.hpp"

using namespace Video;

// Reproducer.
// 
// Compute dispatch 1 - Writes '1' to an SSBO.
// Render pass 1      - Fullscreen triangle.Reads the SSBO, if the value is '1' output green, otherwise red.
// Compute dispatch 2 - Writes '2' to the same SSBO.
// Render pass 2      - Fullscreen triangle.Reads the SSBO, if the value is '2' output green, otherwise red.
bool BugFillBuffer(void* data) {
    assert(data != nullptr);

    // This is my rendering API which is an abstraction on top of Vulkan and OpenGL.
    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    const glm::uvec2 imageSize(128, 128);
    Texture* renderTexture = lowLevelRenderer->CreateRenderTarget(imageSize, Texture::Format::R8G8B8A8);

    // Create shaders.
    // The first shader program writes a value to an SSBO. The value is supplied as a push constant.
    Shader* writeBufferShader = lowLevelRenderer->CreateShader(WRITEBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
    ShaderProgram* writeBufferShaderProgram = lowLevelRenderer->CreateShaderProgram({ writeBufferShader });
    ComputePipeline* writeBufferPipeline = lowLevelRenderer->CreateComputePipeline(writeBufferShaderProgram);

    // The second shader program is a full-screen triangle (hard-coded into the vertex shader, no vertex buffer used).
    // The fragment shader reads back the value from the SSBO and compares it to a push constant. If the values match
    // it outputs green, otherwise red.
    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(VERIFYFILLBUFFER_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create graphics pipeline.
    // Unlike compute pipelines, the Vulkan VkPipeline objects is not created here. It is created when calling
    // BindGraphicsPipeline on the command buffer since render pass information is required. The pipelines are
    // cached and will be re-used if a render pass compatible pipeline is found.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration);

    // Create storage buffer.
    // This buffer will be allocated as one memory allocation. (No sub-allocation from larger buffer.)
    Buffer* zMaskBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(uint32_t));

    // Push constants used by both compute shader and fragment shader.
    struct {
        uint32_t lightCount;
    } pc;

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    // Pipeline barriers will be inserted by the backend as necessary. There are no explicit synchronization commands at this level.
    // Due to the way state is tracked, multiple command buffers can not be recorded in parallel if they touch the same resource.

    // Pass 1.
    {
        // Write to buffer.
        pc.lightCount = 1;

        commandBuffer->BindComputePipeline(writeBufferPipeline);
        commandBuffer->BindStorageBuffers({ zMaskBuffer });
        commandBuffer->PushConstants(&pc);
        commandBuffer->Dispatch(glm::uvec3(1u, 1u, 1u));

        // Use buffer results.
        commandBuffer->BeginRenderPass(renderTexture);
        commandBuffer->BindGraphicsPipeline(graphicsPipeline);
        commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), imageSize);
        commandBuffer->BindStorageBuffers({ zMaskBuffer });
        commandBuffer->PushConstants(&pc);
        commandBuffer->Draw(3);
        commandBuffer->EndRenderPass();
    }

    // Pass 2.
    {
        // Write to buffer.
        pc.lightCount = 2;

        commandBuffer->BindComputePipeline(writeBufferPipeline);
        commandBuffer->BindStorageBuffers({ zMaskBuffer });
        commandBuffer->PushConstants(&pc);
        commandBuffer->Dispatch(glm::uvec3(1u, 1u, 1u));

        // Use buffer results.
        commandBuffer->BeginRenderPass(renderTexture);
        commandBuffer->BindGraphicsPipeline(graphicsPipeline);
        commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), imageSize);
        commandBuffer->BindStorageBuffers({ zMaskBuffer });
        commandBuffer->PushConstants(&pc);
        commandBuffer->Draw(3);
        commandBuffer->EndRenderPass();
    }

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    // In Vulkan: vkDeviceWaitIdle.
    lowLevelRenderer->Wait();

    // Image verification.
    // Reads back the render texture and compares it to a reference image (in this case a fully green image).
    ImageVerification imageVerification(lowLevelRenderer, renderTexture);
    bool result = imageVerification.Compare(BUGFILLBUFFER_PNG, BUGFILLBUFFER_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("BugFillBuffer.png");
    }

    // Cleanup
    delete zMaskBuffer;
    delete commandBuffer;
    delete writeBufferPipeline;
    delete graphicsPipeline;
    lowLevelRenderer->FreeRenderTarget(renderTexture);
    delete writeBufferShaderProgram;
    delete writeBufferShader;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}


// Same as above, but in an infinite loop and presenting to the swap chain instead of rendering off-screen.
// This allows for easier capture with RenderDoc or Radeon GPU Profiler.
bool BugFillBufferLoop(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    const glm::uvec2 imageSize(128, 128);
    Texture* renderTexture = lowLevelRenderer->CreateRenderTarget(imageSize, Texture::Format::R8G8B8A8);
    Texture* renderTexture2 = lowLevelRenderer->CreateRenderTarget(imageSize, Texture::Format::R8G8B8A8);

    // Create shaders.
    Shader* writeBufferShader = lowLevelRenderer->CreateShader(WRITEBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
    ShaderProgram* writeBufferShaderProgram = lowLevelRenderer->CreateShaderProgram({ writeBufferShader });
    ComputePipeline* writeBufferPipeline = lowLevelRenderer->CreateComputePipeline(writeBufferShaderProgram);

    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(VERIFYFILLBUFFER_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration);

    // Create storage buffer.
    Buffer* zMaskBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(uint32_t));

    // Push constants used by both compute shader and fragment shader.
    struct {
        uint32_t lightCount;
    } pc;

    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Create command buffer.
    while (true) {
        lowLevelRenderer->BeginFrame();

        // Pass 1.
        {
            // Write to buffer.
            pc.lightCount = 1;

            commandBuffer->BindComputePipeline(writeBufferPipeline);
            commandBuffer->BindStorageBuffers({ zMaskBuffer });
            commandBuffer->PushConstants(&pc);
            commandBuffer->Dispatch(glm::uvec3(1u, 1u, 1u));

            // Use buffer results.
            commandBuffer->BeginRenderPass(renderTexture);
            commandBuffer->BindGraphicsPipeline(graphicsPipeline);
            commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), imageSize);
            commandBuffer->BindStorageBuffers({ zMaskBuffer });
            commandBuffer->PushConstants(&pc);
            commandBuffer->Draw(3);
            commandBuffer->EndRenderPass();
        }

        // Pass 2.
        {
            // Write to buffer.
            pc.lightCount = 2;

            commandBuffer->BindComputePipeline(writeBufferPipeline);
            commandBuffer->BindStorageBuffers({ zMaskBuffer });
            commandBuffer->PushConstants(&pc);
            commandBuffer->Dispatch(glm::uvec3(1u, 1u, 1u));

            // Use buffer results.
            commandBuffer->BeginRenderPass(renderTexture);
            commandBuffer->BindGraphicsPipeline(graphicsPipeline);
            commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), imageSize);
            commandBuffer->BindStorageBuffers({ zMaskBuffer });
            commandBuffer->PushConstants(&pc);
            commandBuffer->Draw(3);
            commandBuffer->EndRenderPass();
        }

        commandBuffer->BlitToSwapChain(renderTexture);

        // Submit the command buffer.
        lowLevelRenderer->Submit(commandBuffer);
        lowLevelRenderer->Present();
    }

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderTexture);
    bool result = imageVerification.Compare(BUGFILLBUFFER_PNG, BUGFILLBUFFER_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("BugFillBuffer.png");
    }

    // Cleanup
    delete zMaskBuffer;
    delete commandBuffer;
    delete writeBufferPipeline;
    delete graphicsPipeline;
    lowLevelRenderer->FreeRenderTarget(renderTexture);
    delete writeBufferShaderProgram;
    delete writeBufferShader;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}
