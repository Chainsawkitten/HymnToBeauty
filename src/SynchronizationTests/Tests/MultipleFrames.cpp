#include "MultipleFrames.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Framework/Meshes.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PostProcessing.vert.hpp"
#include "Blur.frag.hpp"
#include "CubePush.vert.hpp"
#include "Cube.frag.hpp"

using namespace Video;

const char* MultipleFrames::GetName() const {
    return "Multiple frames";
}

void MultipleFrames::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R8G8B8A8);
    depthBuffer = renderer->CreateRenderTarget(screenSize, Texture::Format::D32);
    commandBuffer = renderer->CreateCommandBuffer();

    // Pipeline to render cubes.
    cubeVertexShader = renderer->CreateShader(CUBEPUSH_VERT, Shader::Type::VERTEX_SHADER);
    cubeFragmentShader = renderer->CreateShader(CUBE_FRAG, Shader::Type::FRAGMENT_SHADER);
    cubeShaderProgram = renderer->CreateShaderProgram({ cubeVertexShader, cubeFragmentShader });

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS_EQUAL;

    vertexDescription = Video::Geometry::VertexType::StaticVertex::GenerateVertexDescription(renderer);

    cubePipeline = renderer->CreateGraphicsPipeline(cubeShaderProgram, configuration, vertexDescription);

    // Cube to render.
    cube = new Meshes::Cube(renderer);
    const glm::uvec2 textureSize(64, 64);
    unsigned char* textureData = new unsigned char[textureSize.x * textureSize.y * 3];
    for (std::size_t i = 0; i < textureSize.x * textureSize.y; ++i) {
        textureData[i * 3 + 0] = i % 255;
        textureData[i * 3 + 1] = 0;
        textureData[i * 3 + 2] = 128;
    }
    cubeAlbedo = renderer->CreateTexture(textureSize, Texture::Format::R8G8B8A8, 3, textureData);
    delete[] textureData;

    // Storage buffer holding instance data.
    struct PerInstance {
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    };

    PerInstance* instances = new PerInstance[instanceCount];

    for (uint32_t i = 0; i < instanceCount; ++i) {
        instances[i].modelMatrix = glm::mat4(1.0f);
        instances[i].modelMatrix[3] = glm::vec4(static_cast<float>(i), 0.0f, 0.0f, 1.0f);
        instances[i].normalMatrix = glm::mat4(1.0f);
    }

    instanceBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(PerInstance) * instanceCount, instances);

    delete[] instances;

    // Uniform buffer holding lighting data.
    struct UniformData {
        uint32_t lightCount = 20;
        float gamma = 1.0f;
    } uniformData;

    uniformBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(uniformData), &uniformData);

    // Storage buffer holding lights.
    struct Light {
        glm::vec4 positionDistance;
        glm::vec4 intensitiesAttenuation;
    };

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    viewMatrix[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);

    Light* lights = new Light[uniformData.lightCount];
    for (uint32_t i = 0; i < uniformData.lightCount; ++i) {
        const float x = static_cast<float>(i % 10) * 2.0f;
        const float y = static_cast<float>(i / 10) * 2.0f;
        const float z = static_cast<float>(i) * 0.5 + 1.0f;
        const glm::vec3 position = viewMatrix * glm::vec4(x, y, z, 1.0f);
        lights[i].positionDistance = glm::vec4(position.x, position.y, position.z, 10.0f);
        lights[i].intensitiesAttenuation = glm::vec4(static_cast<float>(i % 3), static_cast<float>((i + 1) % 3), static_cast<float>((i + 2) % 3), 0.005f);
    }

    lightBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(Light) * uniformData.lightCount, lights);

    delete[] lights;
}

void MultipleFrames::Frame() {
    // Renders a bunch of cubes.
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR, depthBuffer);
    commandBuffer->BindGraphicsPipeline(cubePipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer->BindGeometry(cube->GetGeometryBinding());
    commandBuffer->BindMaterial({ {cubeAlbedo, renderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE)} });

    {
        // Uniform buffer holding matrix data.
        struct Matrices {
            glm::mat4 viewProjectionMatrix;
            glm::mat4 viewMatrix;
        } matrices;

        matrices.viewMatrix = glm::mat4(1.0f);
        matrices.viewMatrix[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
        matrices.viewProjectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(screenSize.x) / screenSize.y, 0.1f, 50.0f) * matrices.viewMatrix;

        Buffer* matrixBuffer = renderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(matrices), &matrices);
        commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matrixBuffer);
    }

    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    commandBuffer->BindStorageBuffers({ instanceBuffer, lightBuffer });

    struct PushConstants {
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    } pushConstants;

    for (uint32_t i = 0; i < instanceCount; ++i) {
        pushConstants.modelMatrix = glm::mat4(1.0f);
        pushConstants.modelMatrix[3] = glm::vec4(static_cast<float>(i), 0.0f, 0.0f, 1.0f);
        pushConstants.normalMatrix = glm::mat4(1.0f);

        commandBuffer->PushConstants(&pushConstants);
        commandBuffer->DrawIndexed(cube->GetIndexCount());
    }

    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void MultipleFrames::Shutdown() {
    delete cubePipeline;
    delete cubeShaderProgram;
    delete cubeVertexShader;
    delete cubeFragmentShader;
    delete vertexDescription;

    delete cube;
    delete cubeAlbedo;
    delete instanceBuffer;
    delete uniformBuffer;
    delete lightBuffer;

    delete mainRenderTarget;
    delete depthBuffer;
    delete commandBuffer;
}
