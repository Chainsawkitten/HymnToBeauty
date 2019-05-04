#include "VideoSuite.hpp"

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
#include <Video/Texture/TexturePNG.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MultipleFrames.vert.hpp"
#include "MultipleFrames.frag.hpp"
#include "Test.png.hpp"

using namespace Video;

bool MultipleFrames(void* data) {
    assert(data != nullptr);

    const uint32_t frameCount = 120;

    LowLevelRenderer* lowLevelRenderer = *static_cast<LowLevelRenderer**>(data);

    // Create render texture.
    Texture* renderTexture = lowLevelRenderer->CreateTexture(glm::uvec2(VideoSuite::swapchainSize, VideoSuite::swapchainSize), Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);

    // Create renderpass.
    RenderPass* renderPass = lowLevelRenderer->CreateRenderPass(renderTexture);

    // Create shaders.
    Shader* vertexShader = lowLevelRenderer->CreateShader(MULTIPLEFRAMES_VERT, Shader::Type::VERTEX_SHADER);
    Shader* fragmentShader = lowLevelRenderer->CreateShader(MULTIPLEFRAMES_FRAG, Shader::Type::FRAGMENT_SHADER);
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
    configuration.blendMode = BlendMode::NONE;
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

    // Create uniform buffers.
    Buffer* uniformBuffer;
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    uniformBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::mat4));

    // Create texture.
    Texture2D* texture = new TexturePNG(lowLevelRenderer, TEST_PNG, TEST_PNG_LENGTH);

    // Create command buffer.
    CommandBuffer* commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    glm::vec3 trianglePosition = glm::vec3(0.0f, 0.0f, 0.0f);

    for (uint32_t frame = 0; frame < frameCount; ++frame) {
        // Update frame.
        trianglePosition.x = cos(static_cast<float>(frame) * 0.05f);
        cameraPosition.y = sin(static_cast<float>(frame) * 0.01f);

        const glm::mat4 viewMatrix = glm::translate(glm::mat4(), -cameraPosition);
        uniformBuffer->Write(&viewMatrix);

        lowLevelRenderer->BeginFrame();

        // Render frame.
        commandBuffer->BeginRenderPass(renderPass);
        commandBuffer->BindGraphicsPipeline(graphicsPipeline);
        commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(VideoSuite::swapchainSize, VideoSuite::swapchainSize));
        commandBuffer->BindGeometry(geometryBinding);
        commandBuffer->BindMaterial({ texture->GetTexture() });
        commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, uniformBuffer);
        const glm::mat4 modelMatrix = glm::translate(glm::mat4(), trianglePosition);
        commandBuffer->PushConstants(&modelMatrix);
        commandBuffer->Draw(3);
        commandBuffer->EndRenderPass();
        commandBuffer->BlitToSwapChain(renderTexture);

        // Submit the command buffer.
        lowLevelRenderer->Submit(commandBuffer);
        lowLevelRenderer->Present();
    }

    lowLevelRenderer->Wait();

    // Cleanup
    delete commandBuffer;
    delete texture;
    delete uniformBuffer;
    delete geometryBinding;
    delete vertexBuffer;
    delete graphicsPipeline;
    delete vertexDescription;
    delete renderPass;
    delete renderTexture;
    delete shaderProgram;
    delete vertexShader;
    delete fragmentShader;

    return true;
}
