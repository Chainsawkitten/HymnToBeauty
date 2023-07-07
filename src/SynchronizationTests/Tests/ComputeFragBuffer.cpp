#include "ComputeFragBuffer.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/ComputePipeline.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Framework/Meshes.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Cube.vert.hpp"
#include "CubeZ.frag.hpp"
#include "LightBinning.comp.hpp"
#include "LightTiling.vert.hpp"
#include "LightTiling.frag.hpp"

using namespace Video;

const char* ComputeFragBuffer::GetName() const {
    return "Compute->Frag (Buffer)";
}

static const uint32_t LIGHT_COUNT = 20;
static const uint32_t BIN_COUNT = 128;

void ComputeFragBuffer::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R8G8B8A8);
    depthBuffer = renderer->CreateRenderTarget(screenSize, Texture::Format::D32);
    commandBuffer = renderer->CreateCommandBuffer();

    // Pipeline to render cubes.
    cubeVertexShader = renderer->CreateShader(CUBE_VERT, Shader::Type::VERTEX_SHADER);
    cubeFragmentShader = renderer->CreateShader(CUBEZ_FRAG, Shader::Type::FRAGMENT_SHADER);
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

    // Pipeline to do light binning.
    binningShader = renderer->CreateShader(LIGHTBINNING_COMP, Shader::Type::COMPUTE_SHADER);
    binningShaderProgram = renderer->CreateShaderProgram({ binningShader });
    binningPipeline = renderer->CreateComputePipeline(binningShaderProgram);

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

    // Uniform buffer holding matrix data.
    struct Matrices {
        glm::mat4 viewProjectionMatrix;
        glm::mat4 viewMatrix;
    } matrices;

    matrices.viewMatrix = glm::mat4(1.0f);
    matrices.viewMatrix[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(screenSize.x) / screenSize.y, 0.1f, 50.0f);
    matrices.viewProjectionMatrix = projectionMatrix * matrices.viewMatrix;

    matrixBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(matrices), &matrices);

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
        uint32_t lightCount = LIGHT_COUNT;
        uint32_t maskCount = (LIGHT_COUNT - 1) / 32 + 1;
        uint32_t zBins = BIN_COUNT;
        float zNear = 0.1f;
        float zFar = 50.0f;
        float gamma = 1.0f;
    } uniformData;

    uniformBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(uniformData), &uniformData);

    // Uniform buffer holding binning data.
    struct BinningData {
        uint32_t lightCount = LIGHT_COUNT;
        uint32_t maskCount = (LIGHT_COUNT - 1) / 32 + 1;
        uint32_t zBins = BIN_COUNT;
        float zNear = 0.1f;
        float zFar = 50.0f;
    } binningData;

    binningUniformBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(binningData), &binningData);

    // Storage buffer holding lights.
    struct Light {
        glm::vec4 positionDistance;
        glm::vec4 intensitiesAttenuation;
    };

    Light* lights = new Light[uniformData.lightCount];
    for (uint32_t i = 0; i < uniformData.lightCount; ++i) {
        float x = static_cast<float>(i % 10) * 2.0f;
        float y = static_cast<float>(i / 10) * 2.0f;
        float z = static_cast<float>(i) * 0.5 + 1.0f;
        glm::vec3 position = matrices.viewMatrix * glm::vec4(x, y, z, 1.0f);
        lights[i].positionDistance = glm::vec4(position.x, position.y, position.z, 10.0f);
        lights[i].intensitiesAttenuation = glm::vec4(static_cast<float>(i % 3), static_cast<float>((i + 1) % 3), static_cast<float>((i + 2) % 3), 0.005f);
    }

    lightBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(Light) * uniformData.lightCount, lights);

    delete[] lights;

    // Storage buffer holding bin masks.
    binBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(uint32_t) * BIN_COUNT);
}

void ComputeFragBuffer::Frame() {
    // Clear tile buffer.
    commandBuffer->ClearBuffer(binBuffer);

    commandBuffer->BindComputePipeline(binningPipeline);
    commandBuffer->BindStorageBuffers({ lightBuffer, binBuffer });
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, binningUniformBuffer);
    const uint32_t threadGroupX = 64;
    commandBuffer->Dispatch(glm::uvec3((LIGHT_COUNT - 1) / threadGroupX + 1, 1, 1));

    // Second render pass renders a bunch of cubes.
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR, depthBuffer);
    commandBuffer->BindGraphicsPipeline(cubePipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer->BindGeometry(cube->GetGeometryBinding());
    commandBuffer->BindMaterial({ {cubeAlbedo, renderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE)} });
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matrixBuffer);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    commandBuffer->BindStorageBuffers({ instanceBuffer, lightBuffer, binBuffer });
    commandBuffer->DrawIndexedInstanced(cube->GetIndexCount(), instanceCount);
    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void ComputeFragBuffer::Shutdown() {
    delete cubePipeline;
    delete cubeShaderProgram;
    delete cubeVertexShader;
    delete cubeFragmentShader;
    delete vertexDescription;

    delete binningPipeline;
    delete binningShaderProgram;
    delete binningShader;

    delete cube;
    delete cubeAlbedo;
    delete matrixBuffer;
    delete instanceBuffer;
    delete uniformBuffer;
    delete lightBuffer;
    delete binBuffer;
    delete binningUniformBuffer;

    delete mainRenderTarget;
    delete depthBuffer;
    delete commandBuffer;
}
