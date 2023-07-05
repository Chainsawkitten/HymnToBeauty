#include "FragFragBuffer.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Video/Lighting/Isocahedron.hpp>
#include <Framework/Meshes.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Cube.vert.hpp"
#include "CubeTiled.frag.hpp"
#include "LightTiling.vert.hpp"
#include "LightTiling.frag.hpp"

using namespace Video;

const char* FragFragBuffer::GetName() const {
    return "Frag->Frag (Buffer)";
}

static const uint32_t LIGHT_COUNT = 20;

void FragFragBuffer::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R8G8B8A8);
    depthBuffer = renderer->CreateRenderTarget(screenSize, Texture::Format::D32);
    commandBuffer = renderer->CreateCommandBuffer();

    // Isocahedron to render when doing light tiling.
    isocahedron = new Isocahedron(renderer);

    // Pipeline to render cubes.
    cubeVertexShader = renderer->CreateShader(CUBE_VERT, Shader::Type::VERTEX_SHADER);
    cubeFragmentShader = renderer->CreateShader(CUBETILED_FRAG, Shader::Type::FRAGMENT_SHADER);
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

    // Pipeline to do light tiling.
    lightTilingVertexShader = renderer->CreateShader(LIGHTTILING_VERT, Shader::Type::VERTEX_SHADER);
    lightTilingFragmentShader = renderer->CreateShader(LIGHTTILING_FRAG, Shader::Type::FRAGMENT_SHADER);
    lightTilingShaderProgram = renderer->CreateShaderProgram({ lightTilingVertexShader, lightTilingFragmentShader });

    configuration.cullFace = CullFace::FRONT;
    configuration.depthMode = DepthMode::NONE;
    configuration.depthClamp = true;

    lightTilingPipeline = renderer->CreateGraphicsPipeline(lightTilingShaderProgram, configuration, isocahedron->GetVertexDescription());

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

    // Uniform buffer holding matrix data during light tiling.
    struct LightMatrices {
        glm::mat4 projectionMatrix;
        float invIsocahedronError;
    } lightMatrices;

    lightMatrices.projectionMatrix = projectionMatrix;
    lightMatrices.invIsocahedronError = 1.0f / isocahedron->GetError();

    tileMatrixBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(lightMatrices), &lightMatrices);

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
        uint32_t tileSize = tileSize;
        uint32_t maskCount = (LIGHT_COUNT - 1) / 32 + 1;
        uint32_t tilesX;
        float gamma = 1.0f;
    } uniformData;

    tiles = (screenSize - glm::uvec2(1, 1)) / glm::uvec2(tileSize, tileSize) + glm::uvec2(1, 1);
    uniformData.tilesX = tiles.x;

    uniformBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(uniformData), &uniformData);

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

    // Storage buffer holding tile masks.
    tileBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(uint32_t) * tiles.x * tiles.y);

    // Uniform buffer holding tiling information.
    struct TilingUniforms {
        uint32_t tileSize = tileSize;
        uint32_t maskCount = (LIGHT_COUNT - 1) / 32 + 1;
        uint32_t tilesX;
    } tilingUniforms;

    tilingUniforms.tilesX = tiles.x;

    tilingUniformBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(tilingUniforms), &tilingUniforms);
}

void FragFragBuffer::Frame() {
    // Clear tile buffer.
    commandBuffer->ClearBuffer(tileBuffer);

    // First render pass tiles lights.
    commandBuffer->BeginAttachmentlessRenderPass(screenSize);
    commandBuffer->BindGraphicsPipeline(lightTilingPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer->BindGeometry(isocahedron->GetGeometryBinding());
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, tileMatrixBuffer);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, tilingUniformBuffer);
    commandBuffer->BindStorageBuffers({ lightBuffer, tileBuffer });
    commandBuffer->DrawIndexedInstanced(isocahedron->GetIndexCount(), LIGHT_COUNT);
    commandBuffer->EndRenderPass();

    // Second render pass renders a bunch of cubes.
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR, depthBuffer);
    commandBuffer->BindGraphicsPipeline(cubePipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer->BindGeometry(cube->GetGeometryBinding());
    commandBuffer->BindMaterial({ {cubeAlbedo, renderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE)} });
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matrixBuffer);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    commandBuffer->BindStorageBuffers({ instanceBuffer, lightBuffer, tileBuffer });
    commandBuffer->DrawIndexedInstanced(cube->GetIndexCount(), instanceCount);
    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void FragFragBuffer::Shutdown() {
    delete cubePipeline;
    delete cubeShaderProgram;
    delete cubeVertexShader;
    delete cubeFragmentShader;
    delete vertexDescription;

    delete lightTilingPipeline;
    delete lightTilingShaderProgram;
    delete lightTilingVertexShader;
    delete lightTilingFragmentShader;

    delete isocahedron;
    delete cube;
    delete cubeAlbedo;
    delete matrixBuffer;
    delete instanceBuffer;
    delete uniformBuffer;
    delete lightBuffer;
    delete tileBuffer;
    delete tileMatrixBuffer;
    delete tilingUniformBuffer;

    delete mainRenderTarget;
    delete depthBuffer;
    delete commandBuffer;
}
