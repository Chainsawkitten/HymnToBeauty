#include "VideoSuite.hpp"

#include "ImageVerification.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/RenderPass.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/LowLevelRenderer/Interface/VertexDescription.hpp>
#include <Video/LowLevelRenderer/Interface/GeometryBinding.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/ComputePipeline.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/Texture/Texture2D.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "FullscreenTriangle.vert.hpp"
#include "StorageBuffer.frag.hpp"
#include "SetBuffer.comp.hpp"
#include "ComputeSetBuffer.png.hpp"

using namespace Video;

static const unsigned int imageSize = 64;

bool ComputeSetBuffer(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(STORAGEBUFFER_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* graphicsShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    Shader* computeShader = lowLevelRenderer->CreateShader(SETBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
    ShaderProgram* computeShaderProgram = lowLevelRenderer->CreateShaderProgram({ computeShader });

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(graphicsShaderProgram, configuration);

    // Create compute pipeline.
    ComputePipeline* computePipeline = lowLevelRenderer->CreateComputePipeline(computeShaderProgram);

    // Create storage buffer.
    const glm::vec4 storageBufferData = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    Buffer* storageBuffer = lowLevelRenderer->CreateBuffer(Video::Buffer::BufferUsage::STORAGE_BUFFER, sizeof(glm::vec4), &storageBufferData);

    // Create uniform buffer.
    const glm::vec4 uniformBufferData = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    Buffer* uniformBuffer = lowLevelRenderer->CreateBuffer(Video::Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::vec4), &uniformBufferData);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Set buffer contents.
    commandBuffer->BindComputePipeline(computePipeline);
    commandBuffer->BindStorageBuffer(storageBuffer);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    const glm::vec4 pushConstantData = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    commandBuffer->PushConstants(&pushConstantData);
    commandBuffer->Dispatch(glm::uvec3(1, 1, 1));

    // Output buffer contents to render target.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindStorageBuffer(storageBuffer);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(COMPUTESETBUFFER_PNG, COMPUTESETBUFFER_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("ComputeSetBuffer.png");
    }

    // Cleanup
    delete commandBuffer;
    delete storageBuffer;
    delete uniformBuffer;
    delete graphicsPipeline;
    delete computePipeline;
    delete renderPass;
    delete colorTexture;
    delete graphicsShaderProgram;
    delete vertexShader;
    delete fragmentShader;
    delete computeShaderProgram;
    delete computeShader;

    return result;
}
