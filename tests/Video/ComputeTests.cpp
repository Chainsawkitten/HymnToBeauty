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
#include "ComputeSetBuffer.comp.hpp"
#include "ComputeSetBuffer.png.hpp"

#include "ComputeVertexBuffer.comp.hpp"
#include "ComputeVertexBuffer.vert.hpp"
#include "DrawTriangle.frag.hpp"
#include "DrawTriangle.png.hpp"

#include "ComputeCopyBuffer.comp.hpp"

#include "ComputeClearBuffer.png.hpp"

using namespace Video;

static const unsigned int imageSize = 64;

bool ComputeSetBuffer(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateRenderTarget(glm::uvec2(imageSize, imageSize), Texture::Format::R8G8B8A8);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(STORAGEBUFFER_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* graphicsShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    Shader* computeShader = lowLevelRenderer->CreateShader(COMPUTESETBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
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
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    const glm::vec4 pushConstantData = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    commandBuffer->PushConstants(&pushConstantData);
    commandBuffer->Dispatch(glm::uvec3(1, 1, 1));

    // Output buffer contents to render target.
    commandBuffer->BeginRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, colorTexture);
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
    lowLevelRenderer->FreeRenderTarget(colorTexture);
    delete graphicsShaderProgram;
    delete vertexShader;
    delete fragmentShader;
    delete computeShaderProgram;
    delete computeShader;

    return result;
}

bool ComputeVertexBuffer(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateRenderTarget(glm::uvec2(imageSize, imageSize), Texture::Format::R8G8B8A8);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(COMPUTEVERTEXBUFFER_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* graphicsShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    Shader* computeShader = lowLevelRenderer->CreateShader(COMPUTEVERTEXBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
    ShaderProgram* computeShaderProgram = lowLevelRenderer->CreateShaderProgram({ computeShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 4;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Color.
        attributes[1].size = 4;
        attributes[1].type = VertexDescription::AttributeType::FLOAT;
        attributes[1].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(2, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;
    configuration.depthComparison = DepthComparison::ALWAYS;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(graphicsShaderProgram, configuration, vertexDescription);

    // Create compute pipeline.
    ComputePipeline* computePipeline = lowLevelRenderer->CreateComputePipeline(computeShaderProgram);

    // Create vertex/storage buffer.
    Buffer* storageBuffer = lowLevelRenderer->CreateBuffer(Video::Buffer::BufferUsage::VERTEX_STORAGE_BUFFER, sizeof(glm::vec4) * 6);

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, storageBuffer);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Set buffer contents.
    commandBuffer->BindComputePipeline(computePipeline);
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->Dispatch(glm::uvec3(1, 1, 1));

    // Draw triangle using the vertex buffer written to by the compute pass.
    commandBuffer->BeginRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, colorTexture);
    bool result = imageVerification.Compare(DRAWTRIANGLE_PNG, DRAWTRIANGLE_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("ComputeVertexBuffer.png");
    }

    // Cleanup
    delete commandBuffer;
    delete geometryBinding;
    delete vertexDescription;
    delete storageBuffer;
    delete graphicsPipeline;
    delete computePipeline;
    lowLevelRenderer->FreeRenderTarget(colorTexture);
    delete graphicsShaderProgram;
    delete vertexShader;
    delete fragmentShader;
    delete computeShaderProgram;
    delete computeShader;

    return result;
}

bool ComputeMultipleBuffers(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateRenderTarget(glm::uvec2(imageSize, imageSize), Texture::Format::R8G8B8A8);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(STORAGEBUFFER_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* graphicsShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    Shader* computeShader = lowLevelRenderer->CreateShader(COMPUTECOPYBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
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

    // Create source buffer.
    const glm::vec4 sourceData = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    Buffer* sourceBuffer = lowLevelRenderer->CreateBuffer(Video::Buffer::BufferUsage::STORAGE_BUFFER, sizeof(glm::vec4), &sourceData);

    // Create destination buffer.
    const glm::vec4 destinationData = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    Buffer* destinationBuffer = lowLevelRenderer->CreateBuffer(Video::Buffer::BufferUsage::STORAGE_BUFFER, sizeof(glm::vec4), &destinationData);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Set buffer contents.
    commandBuffer->BindComputePipeline(computePipeline);
    commandBuffer->BindStorageBuffers({ sourceBuffer, destinationBuffer });
    commandBuffer->Dispatch(glm::uvec3(1, 1, 1));

    // Output buffer contents to render target.
    commandBuffer->BeginRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindStorageBuffers({ destinationBuffer });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, colorTexture);
    bool result = imageVerification.Compare(COMPUTESETBUFFER_PNG, COMPUTESETBUFFER_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("ComputeMultipleBuffers.png");
    }

    // Cleanup
    delete commandBuffer;
    delete sourceBuffer;
    delete destinationBuffer;
    delete graphicsPipeline;
    delete computePipeline;
    lowLevelRenderer->FreeRenderTarget(colorTexture);
    delete graphicsShaderProgram;
    delete vertexShader;
    delete fragmentShader;
    delete computeShaderProgram;
    delete computeShader;

    return result;
}

bool ComputeClearBuffer(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateRenderTarget(glm::uvec2(imageSize, imageSize), Texture::Format::R8G8B8A8);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(STORAGEBUFFER_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* graphicsShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    Shader* computeShader = lowLevelRenderer->CreateShader(COMPUTESETBUFFER_COMP, Shader::Type::COMPUTE_SHADER);
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
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    const glm::vec4 pushConstantData = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    commandBuffer->PushConstants(&pushConstantData);
    commandBuffer->Dispatch(glm::uvec3(1, 1, 1));

    commandBuffer->ClearBuffer(storageBuffer);

    // Output buffer contents to render target.
    commandBuffer->BeginRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, colorTexture);
    bool result = imageVerification.Compare(COMPUTECLEARBUFFER_PNG, COMPUTECLEARBUFFER_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("ComputeClearBuffer.png");
    }

    // Cleanup
    delete commandBuffer;
    delete storageBuffer;
    delete uniformBuffer;
    delete graphicsPipeline;
    delete computePipeline;
    lowLevelRenderer->FreeRenderTarget(colorTexture);
    delete graphicsShaderProgram;
    delete vertexShader;
    delete fragmentShader;
    delete computeShaderProgram;
    delete computeShader;

    return result;
}
