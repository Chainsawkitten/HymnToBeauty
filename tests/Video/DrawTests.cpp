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
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/Texture/Texture2D.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DrawTriangle.vert.hpp"
#include "DrawTriangle.frag.hpp"
#include "DrawTriangle.png.hpp"

#include "DrawVertexTriangle.vert.hpp"

#include "DrawTexturedTriangle.vert.hpp"
#include "DrawTexturedTriangle.frag.hpp"
#include "DrawTexturedTriangle.png.hpp"
#include "Test.png.hpp"

#include "DrawQuad.png.hpp"

#include "DrawTriangles.vert.hpp"
#include "DrawTriangles.png.hpp"

#include "DrawPushTriangles.vert.hpp"
#include "DrawPushTriangles.png.hpp"

#include "DrawStorageTriangle.vert.hpp"

#include "FullscreenTriangle.vert.hpp"
#include "InvertColors.frag.hpp"
#include "InvertColors.png.hpp"

#include "DrawMipmappedTriangle.frag.hpp"
#include "DrawMipmappedTriangle.png.hpp"

#include "DepthPrePass.png.hpp"

#include "DrawLines.png.hpp"

#include "AttachmentlessWrite.frag.hpp"
#include "AttachmentlessRead.frag.hpp"
#include "Attachmentless.png.hpp"

using namespace Video;

static const unsigned int imageSize = 64;

bool DrawTriangle(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWTRIANGLE_PNG, DRAWTRIANGLE_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawTriangle.png");
    }

    // Cleanup
    delete commandBuffer;
    delete graphicsPipeline;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawVertexTriangle(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWVERTEXTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Color.
        attributes[1].size = 3;
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

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z, r, g, b
        float vertexData[] = {
             0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
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
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWTRIANGLE_PNG, DRAWTRIANGLE_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawVertexTriangle.png");
    }

    // Cleanup
    delete commandBuffer;
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawTexturedTriangle(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWTEXTUREDTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTEXTUREDTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Texture coordinates.
        attributes[1].size = 2;
        attributes[1].type = VertexDescription::AttributeType::FLOAT;
        attributes[1].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(2, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z, u, v
        float vertexData[] = {
             0.0f, -1.0f, 0.0f, 0.5f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 0.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Create texture.
    Texture2D* texture = new Texture2D(lowLevelRenderer, TEST_PNG, TEST_PNG_LENGTH);
    Texture2D* texture2 = new Texture2D(lowLevelRenderer, TEST_PNG, TEST_PNG_LENGTH);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->BindMaterial({ texture->GetTexture(), texture2->GetTexture() });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWTEXTUREDTRIANGLE_PNG, DRAWTEXTUREDTRIANGLE_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawTexturedTriangle.png");
    }

    // Cleanup
    delete commandBuffer;
    delete texture;
    delete texture2;
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawQuad(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWVERTEXTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Color.
        attributes[1].size = 3;
        attributes[1].type = VertexDescription::AttributeType::FLOAT;
        attributes[1].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(2, attributes, true);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z, r, g, b
        float vertexData[] = {
            -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create index buffer.
    Buffer* indexBuffer;
    {
        uint16_t indexData[] = {
            0, 2, 1,
            2, 3, 1
        };

        indexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER_STATIC, sizeof(indexData), indexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer, GeometryBinding::IndexType::SHORT, indexBuffer);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->DrawIndexed(6);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWQUAD_PNG, DRAWQUAD_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawQuad.png");
    }

    // Cleanup
    delete commandBuffer;
    delete geometryBinding;
    delete vertexBuffer;
    delete indexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawTriangles(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);
    Texture* depthTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_DEPTH, Texture::Format::D32);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR, depthTexture, RenderPass::LoadOperation::CLEAR);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWTRIANGLES_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Color.
        attributes[1].size = 3;
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
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z, r, g, b
        float vertexData[] = {
             0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Create uniform buffers.
    Buffer* uniformBuffers[2];
    glm::vec3 positions[2] = { glm::vec3(0.2f, 0.0f, -0.5f), glm::vec3(-0.2f, 0.0f, 0.5f) };
    for (int i = 0; i < 2; ++i) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(), positions[i]);
        uniformBuffers[i] = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::mat4), &modelMatrix);
    }

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, uniformBuffers[0]);
    commandBuffer->Draw(3);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, uniformBuffers[1]);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWTRIANGLES_PNG, DRAWTRIANGLES_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawTriangles.png");
    }

    // Cleanup
    delete commandBuffer;
    delete uniformBuffers[0];
    delete uniformBuffers[1];
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete colorTexture;
    delete depthTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawPushTriangles(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);
    Texture* depthTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_DEPTH, Texture::Format::D32);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR, depthTexture, RenderPass::LoadOperation::CLEAR);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWPUSHTRIANGLES_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(1, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z
        float vertexData[] = {
             0.0f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Push constants.
    struct {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    } pushConstants[2];
    pushConstants[0].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0.2f, 0.0f, -0.5f));
    pushConstants[0].color = glm::vec3(1.0, 0.0, 0.0);
    pushConstants[1].modelMatrix = glm::translate(glm::mat4(), glm::vec3(-0.2f, 0.0f, 0.5f));
    pushConstants[1].color = glm::vec3(0.0, 1.0, 0.0);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->PushConstants(&pushConstants[0]);
    commandBuffer->Draw(3);
    commandBuffer->PushConstants(&pushConstants[1]);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWPUSHTRIANGLES_PNG, DRAWPUSHTRIANGLES_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawPushTriangles.png");
    }

    // Cleanup
    delete commandBuffer;
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete colorTexture;
    delete depthTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawStorageTriangle(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWSTORAGETRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
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
    struct StorageContents {
        glm::vec4 colors[3];
        glm::vec2 positions[3];
    } storageContents;

    storageContents.positions[0] = glm::vec2(0.0f, -0.5f);
    storageContents.positions[1] = glm::vec2(0.5f, 0.5f);
    storageContents.positions[2] = glm::vec2(-0.5f, 0.5f);
    storageContents.colors[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    storageContents.colors[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    storageContents.colors[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    
    Buffer* storageBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(StorageContents), &storageContents);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
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
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWTRIANGLE_PNG, DRAWTRIANGLE_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawStorageTriangle.png");
    }

    // Cleanup
    delete commandBuffer;
    delete storageBuffer;
    delete graphicsPipeline;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool InvertColors(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);
    Texture* depthTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_DEPTH, Texture::Format::D32);
    Texture* secondColorTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* firstRenderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR, depthTexture, RenderPass::LoadOperation::CLEAR);
    RenderPass* secondRenderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::LOAD, depthTexture, RenderPass::LoadOperation::LOAD);
    RenderPass* invertRenderPass = lowLevelRenderer->CreateRenderPass(secondColorTexture, RenderPass::LoadOperation::DONT_CARE);

    // Create shaders.
    Shader* triangleVertexShader = lowLevelRenderer->CreateShader(DRAWPUSHTRIANGLES_VERT, Shader::Type::VERTEX_SHADER);
    Shader* triangleFragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* triangleShaderProgram = lowLevelRenderer->CreateShaderProgram({ triangleVertexShader, triangleFragmentShader });

    Shader* invertVertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* invertFragmentShader = lowLevelRenderer->CreateShader(INVERTCOLORS_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* invertShaderProgram = lowLevelRenderer->CreateShaderProgram({ invertVertexShader, invertFragmentShader });

    // Create vertex descriptions.
    VertexDescription* triangleVertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[1];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        triangleVertexDescription = lowLevelRenderer->CreateVertexDescription(1, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS;

    GraphicsPipeline* triangleGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(triangleShaderProgram, configuration, triangleVertexDescription);

    configuration.cullFace = CullFace::NONE;
    configuration.depthMode = DepthMode::NONE;
    GraphicsPipeline* invertGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(invertShaderProgram, configuration);

    // Create triangle vertex buffer.
    Buffer* triangleVertexBuffer;
    {
        // x, y, z
        float vertexData[] = {
             0.0f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        triangleVertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* triangleGeometryBinding = lowLevelRenderer->CreateGeometryBinding(triangleVertexDescription, triangleVertexBuffer);

    // Push constants.
    struct {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    } pushConstants[2];
    pushConstants[0].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0.2f, 0.0f, -0.5f));
    pushConstants[0].color = glm::vec3(1.0, 0.0, 0.0);
    pushConstants[1].modelMatrix = glm::translate(glm::mat4(), glm::vec3(-0.2f, 0.0f, 0.5f));
    pushConstants[1].color = glm::vec3(0.0, 1.0, 0.0);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(firstRenderPass);
    commandBuffer->BindGraphicsPipeline(triangleGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(triangleGeometryBinding);
    commandBuffer->PushConstants(&pushConstants[0]);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    commandBuffer->BeginRenderPass(secondRenderPass);
    commandBuffer->BindGraphicsPipeline(triangleGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(triangleGeometryBinding);
    commandBuffer->PushConstants(&pushConstants[1]);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    commandBuffer->BeginRenderPass(invertRenderPass);
    commandBuffer->BindGraphicsPipeline(invertGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindMaterial({ colorTexture });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, invertRenderPass);
    bool result = imageVerification.Compare(INVERTCOLORS_PNG, INVERTCOLORS_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("InvertColors.png");
    }

    // Cleanup
    delete commandBuffer;
    delete triangleGeometryBinding;
    delete triangleVertexBuffer;
    delete triangleGraphicsPipeline;
    delete invertGraphicsPipeline;
    delete triangleVertexDescription;
    delete firstRenderPass;
    delete secondRenderPass;
    delete invertRenderPass;
    delete colorTexture;
    delete depthTexture;
    delete secondColorTexture;
    delete triangleShaderProgram;
    delete triangleVertexShader;
    delete triangleFragmentShader;
    delete invertShaderProgram;
    delete invertVertexShader;
    delete invertFragmentShader;

    return result;
}

bool DrawMipmappedTriangle(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWTEXTUREDTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWMIPMAPPEDTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Texture coordinates.
        attributes[1].size = 2;
        attributes[1].type = VertexDescription::AttributeType::FLOAT;
        attributes[1].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(2, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z, u, v
        float vertexData[] = {
             0.0f, -1.0f, 0.0f, 0.5f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 0.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Create texture.
    Texture2D* texture = new Texture2D(lowLevelRenderer, TEST_PNG, TEST_PNG_LENGTH);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->BindMaterial({ texture->GetTexture() });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWMIPMAPPEDTRIANGLE_PNG, DRAWMIPMAPPEDTRIANGLE_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DrawMipmappedTriangle.png");
    }

    // Cleanup
    delete commandBuffer;
    delete texture;
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DepthPrePass(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render textures.
    Texture* colorTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);
    Texture* depthTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_DEPTH, Texture::Format::D32);

    // Create renderpass.
    RenderPass* depthRenderPass = lowLevelRenderer->CreateRenderPass(nullptr, RenderPass::LoadOperation::DONT_CARE, depthTexture, RenderPass::LoadOperation::CLEAR);
    RenderPass* colorRenderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR, depthTexture, RenderPass::LoadOperation::LOAD);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWPUSHTRIANGLES_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(1, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS;

    GraphicsPipeline* depthGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    configuration.depthMode = DepthMode::TEST;
    configuration.depthComparison = DepthComparison::LESS_EQUAL;

    GraphicsPipeline* colorGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z
        float vertexData[] = {
             0.0f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Push constants.
    struct {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    } pushConstants[2];
    pushConstants[0].modelMatrix = glm::translate(glm::mat4(), glm::vec3(0.2f, 0.0f, -0.5f));
    pushConstants[0].color = glm::vec3(1.0, 0.0, 0.0);
    pushConstants[1].modelMatrix = glm::translate(glm::mat4(), glm::vec3(-0.2f, 0.0f, 0.5f));
    pushConstants[1].color = glm::vec3(0.0, 1.0, 0.0);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(depthRenderPass);
    commandBuffer->BindGraphicsPipeline(depthGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->PushConstants(&pushConstants[0]);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    commandBuffer->BeginRenderPass(colorRenderPass);
    commandBuffer->BindGraphicsPipeline(colorGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->PushConstants(&pushConstants[1]);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, colorRenderPass);
    bool result = imageVerification.Compare(DEPTHPREPASS_PNG, DEPTHPREPASS_PNG_LENGTH);
    if (!result) {
        imageVerification.WritePNG("DepthPrePass.png");
    }

    // Cleanup
    delete commandBuffer;
    delete geometryBinding;
    delete vertexBuffer;
    delete depthGraphicsPipeline;
    delete colorGraphicsPipeline;
    delete vertexDescription;
    delete depthRenderPass;
    delete colorRenderPass;
    delete colorTexture;
    delete depthTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool DrawLines(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(DRAWVERTEXTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(DRAWTRIANGLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* shaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, fragmentShader });

    // Create vertex description.
    VertexDescription* vertexDescription;
    {
        // Position.
        VertexDescription::Attribute attributes[2];
        attributes[0].size = 3;
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].normalized = false;

        // Color.
        attributes[1].size = 3;
        attributes[1].type = VertexDescription::AttributeType::FLOAT;
        attributes[1].normalized = false;

        vertexDescription = lowLevelRenderer->CreateVertexDescription(2, attributes);
    }

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::LINE;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    // Create vertex buffer.
    Buffer* vertexBuffer;
    {
        // x, y, z, r, g, b
        float vertexData[] = {
             0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(vertexData), vertexData);
    }

    // Create geometry binding.
    GeometryBinding* geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(graphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->BindGeometry(geometryBinding);
    commandBuffer->SetLineWidth(3.0f);
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(DRAWLINES_PNG, DRAWLINES_PNG_LENGTH, 1);
    if (!result) {
        imageVerification.WritePNG("DrawLines.png");
    }

    // Cleanup
    delete commandBuffer;
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return result;
}

bool Attachmentless(void* data) {
    assert(data != nullptr);

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(imageSize, imageSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* attachmentlessRenderPass = lowLevelRenderer->CreateAttachmentlessRenderPass(renderTexture->GetSize());
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(FULLSCREENTRIANGLE_VERT, Shader::Type::VERTEX_SHADER);
    Shader* writeFragmentShader = lowLevelRenderer->CreateShader(ATTACHMENTLESSWRITE_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* writeShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, writeFragmentShader });

    Shader* readFragmentShader = lowLevelRenderer->CreateShader(ATTACHMENTLESSREAD_FRAG, Shader::Type::FRAGMENT_SHADER);
    ShaderProgram* readShaderProgram = lowLevelRenderer->CreateShaderProgram({ vertexShader, readFragmentShader });

    // Create graphics pipeline.
    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    GraphicsPipeline* writeGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(writeShaderProgram, configuration);
    GraphicsPipeline* readGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(readShaderProgram, configuration);

    // Create storage buffer.
    Buffer* storageBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(glm::vec4) * imageSize * imageSize);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    // Record command buffer.
    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(writeGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    const glm::uvec2 screenSize(imageSize, imageSize);
    commandBuffer->PushConstants(&screenSize);
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    commandBuffer->BeginRenderPass(renderPass);
    commandBuffer->BindGraphicsPipeline(readGraphicsPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(imageSize, imageSize));
    commandBuffer->PushConstants(&screenSize);
    commandBuffer->BindStorageBuffers({ storageBuffer });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    // Submit the command buffer.
    lowLevelRenderer->Submit(commandBuffer);

    // Wait for rendering to finish.
    lowLevelRenderer->Wait();

    // Image verification.
    ImageVerification imageVerification(lowLevelRenderer, renderPass);
    bool result = imageVerification.Compare(ATTACHMENTLESS_PNG, ATTACHMENTLESS_PNG_LENGTH, 1);
    if (!result) {
        imageVerification.WritePNG("Attachmentless.png");
    }

    // Cleanup
    delete commandBuffer;
    delete writeGraphicsPipeline;
    delete readGraphicsPipeline;
    delete attachmentlessRenderPass;
    delete renderPass;
    delete renderTexture;
    delete writeShaderProgram;
    delete readShaderProgram;
    delete vertexShader;
    delete writeFragmentShader;
    delete readFragmentShader;
    delete storageBuffer;

    return result;
}
