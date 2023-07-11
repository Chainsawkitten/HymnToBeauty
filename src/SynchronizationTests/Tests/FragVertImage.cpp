#include "FragVertImage.hpp"

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
#include <cmath>

#include "Plane.vert.hpp"
#include "Plane.frag.hpp"
#include "Offset.vert.hpp"
#include "Offset.frag.hpp"

using namespace Video;

static const uint32_t planeDivisions = 100;
static const uint32_t offsetInstances = 10;

const char* FragVertImage::GetName() const {
    return "Frag->Vert (Image)";
}

void FragVertImage::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R8G8B8A8);
    depthBuffer = renderer->CreateRenderTarget(screenSize, Texture::Format::D32);
    offsetBuffer = renderer->CreateRenderTarget(glm::uvec2(planeDivisions, planeDivisions), Texture::Format::R8);
    commandBuffer = renderer->CreateCommandBuffer();

    // Pipeline to render cubes.
    planeVertexShader = renderer->CreateShader(PLANE_VERT, Shader::Type::VERTEX_SHADER);
    planeFragmentShader = renderer->CreateShader(PLANE_FRAG, Shader::Type::FRAGMENT_SHADER);
    planeShaderProgram = renderer->CreateShaderProgram({ planeVertexShader, planeFragmentShader });

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST_WRITE;
    configuration.depthComparison = DepthComparison::LESS_EQUAL;

    vertexDescription = Geometry::VertexType::StaticVertex::GenerateVertexDescription(renderer);

    planePipeline = renderer->CreateGraphicsPipeline(planeShaderProgram, configuration, vertexDescription);

    // Pipeline to render offsets.
    offsetVertexShader = renderer->CreateShader(OFFSET_VERT, Shader::Type::VERTEX_SHADER);
    offsetFragmentShader = renderer->CreateShader(OFFSET_FRAG, Shader::Type::FRAGMENT_SHADER);
    offsetShaderProgram = renderer->CreateShaderProgram({ offsetVertexShader, offsetFragmentShader });

    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::ONE_ONE_MINUS_SRC_ALPHA;
    configuration.depthMode = DepthMode::NONE;

    {
        VertexDescription::Attribute attributes[2];
        attributes[0].type = VertexDescription::AttributeType::FLOAT;
        attributes[0].size = 3;
        attributes[0].normalized = false;

        attributes[1].type = VertexDescription::AttributeType::FLOAT;
        attributes[1].size = 2;
        attributes[1].normalized = false;

        offsetVertexDescription = renderer->CreateVertexDescription(2, attributes, true);
    }

    offsetPipeline = renderer->CreateGraphicsPipeline(offsetShaderProgram, configuration, offsetVertexDescription);

    // Plane to render.
    Geometry::VertexType::StaticVertex* vertices = new Geometry::VertexType::StaticVertex[planeDivisions * planeDivisions];
    for (uint32_t x = 0; x < planeDivisions; ++x) {
        for (uint32_t y = 0; y < planeDivisions; ++y) {
            const uint32_t index = x + y * planeDivisions;
            vertices[index].textureCoordinate = glm::vec2(static_cast<float>(x), static_cast<float>(y)) / static_cast<float>(planeDivisions - 1);
            vertices[index].position = glm::vec3(vertices[index].textureCoordinate.x, 0.0f, vertices[index].textureCoordinate.y);
            vertices[index].normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertices[index].tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }

    planeVertexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, sizeof(Geometry::VertexType::StaticVertex) * planeDivisions * planeDivisions, vertices);
    delete[] vertices;

    planeIndexCount = (planeDivisions - 1) * (planeDivisions - 1) * 6;
    uint16_t* indices = new uint16_t[planeIndexCount];
    for (uint32_t x = 0; x < planeDivisions - 1; ++x) {
        for (uint32_t y = 0; y < planeDivisions - 1; ++y) {
            const uint32_t index = (x + y * (planeDivisions - 1)) * 6;
            indices[index + 0] = x + 0 + (y + 0) * planeDivisions;
            indices[index + 1] = x + 0 + (y + 1) * planeDivisions;
            indices[index + 2] = x + 1 + (y + 0) * planeDivisions;
            indices[index + 3] = x + 1 + (y + 0) * planeDivisions;
            indices[index + 4] = x + 0 + (y + 1) * planeDivisions;
            indices[index + 5] = x + 1 + (y + 1) * planeDivisions;
        }
    }

    planeIndexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, sizeof(uint16_t) * planeIndexCount, indices);
    delete[] indices;

    planeGeometryBinding = renderer->CreateGeometryBinding(vertexDescription, planeVertexBuffer, GeometryBinding::IndexType::SHORT, planeIndexBuffer);

    const glm::uvec2 textureSize(64, 64);
    unsigned char* textureData = new unsigned char[textureSize.x * textureSize.y * 3];
    for (std::size_t i = 0; i < textureSize.x * textureSize.y; ++i) {
        textureData[i * 3 + 0] = i % 255;
        textureData[i * 3 + 1] = 0;
        textureData[i * 3 + 2] = 128;
    }
    planeAlbedo = renderer->CreateTexture(textureSize, Texture::Format::R8G8B8A8, 3, textureData);
    delete[] textureData;

    // Offset square.
    struct OffsetVertex {
        glm::vec3 position;
        glm::vec2 texCoords;
    };

    OffsetVertex offsetVertices[4] = {
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f)}
    };

    offsetVertexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, sizeof(OffsetVertex) * 4, offsetVertices);

    uint16_t offsetIndices[6] = {
        0, 2, 1,
        1, 2, 3
    };

    offsetIndexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, sizeof(uint16_t) * 6, offsetIndices);
    offsetGeometryBinding = renderer->CreateGeometryBinding(offsetVertexDescription, offsetVertexBuffer, GeometryBinding::IndexType::SHORT, offsetIndexBuffer);

    unsigned char* offsetTextureData = new unsigned char[planeDivisions * planeDivisions];
    for (uint32_t x = 0; x < planeDivisions; ++x) {
        for (uint32_t y = 0; y < planeDivisions; ++y) {
            glm::vec2 pos = glm::vec2(static_cast<float>(x), static_cast<float>(y)) / static_cast<float>(planeDivisions - 1) - glm::vec2(0.5f, 0.5f);
            offsetTextureData[x + y * planeDivisions] = std::max(static_cast<int32_t>(128.0f - sqrt(pos.x * pos.x + pos.y * pos.y) * 255.0f), 0);
        }
    }
    offsetTexture = renderer->CreateTexture(glm::uvec2(planeDivisions, planeDivisions), Texture::Format::R8G8B8A8, 1, offsetTextureData);
    delete[] offsetTextureData;

    // Offset instance buffer.
    glm::mat4 offsetMatrices[offsetInstances];
    for (uint32_t i = 0; i < offsetInstances; ++i) {
        offsetMatrices[i] = glm::mat4(1.0f);
        offsetMatrices[i][3] = glm::vec4(static_cast<float>(i) / 10.0f, 0.0f, 0.0f, 1.0f);
    }

    offsetInstanceBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(glm::mat4) * offsetInstances, offsetMatrices);

    // Uniform buffer holding matrix data.
    struct Matrices {
        glm::mat4 viewProjectionMatrix;
        glm::mat4 viewMatrix;
    } matrices;

    matrices.viewMatrix = glm::mat4(1.0f);
    matrices.viewMatrix[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
    matrices.viewProjectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(screenSize.x) / screenSize.y, 0.1f, 50.0f) * matrices.viewMatrix;

    matrixBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(matrices), &matrices);

    // Storage buffer holding instance data.
    struct PerInstance {
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    } instance;

    instance.modelMatrix = glm::mat4(1.0f);
    const float scale = 8.0f;
    instance.modelMatrix[0] *= scale;
    instance.modelMatrix[1] *= scale;
    instance.modelMatrix[2] *= scale;
    instance.modelMatrix[3] = glm::vec4(-4.0f, -1.0f, 0.0f, 1.0f);
    instance.normalMatrix = glm::mat4(1.0f);

    instanceBuffer = renderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(PerInstance), &instance);

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
}

void FragVertImage::Frame() {
    // First render pass renders offset.
    commandBuffer->BeginRenderPass(offsetBuffer);
    commandBuffer->BindGraphicsPipeline(offsetPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(planeDivisions, planeDivisions));
    commandBuffer->BindGeometry(offsetGeometryBinding);
    commandBuffer->BindMaterial({
        {offsetTexture, renderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE)},
        });
    commandBuffer->BindStorageBuffers({offsetInstanceBuffer});
    commandBuffer->DrawIndexedInstanced(6, offsetInstances);
    commandBuffer->EndRenderPass();

    // Second render pass renders a plane.
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR, depthBuffer);
    commandBuffer->BindGraphicsPipeline(planePipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer->BindGeometry(planeGeometryBinding);
    commandBuffer->BindMaterial({
        {planeAlbedo, renderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE)},
        {offsetBuffer, renderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE)}
        });
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matrixBuffer);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uniformBuffer);
    commandBuffer->BindStorageBuffers({ instanceBuffer, lightBuffer });
    commandBuffer->DrawIndexed(planeIndexCount);
    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void FragVertImage::Shutdown() {
    delete offsetPipeline;
    delete offsetShaderProgram;
    delete offsetVertexShader;
    delete offsetFragmentShader;

    delete planePipeline;
    delete planeShaderProgram;
    delete planeVertexShader;
    delete planeFragmentShader;
    delete vertexDescription;

    delete planeGeometryBinding;
    delete planeVertexBuffer;
    delete planeIndexBuffer;
    delete planeAlbedo;
    delete matrixBuffer;
    delete instanceBuffer;
    delete uniformBuffer;
    delete lightBuffer;

    delete offsetGeometryBinding;
    delete offsetVertexBuffer;
    delete offsetIndexBuffer;
    delete offsetVertexDescription;
    delete offsetTexture;
    delete offsetInstanceBuffer;

    delete mainRenderTarget;
    delete depthBuffer;
    delete offsetBuffer;
    delete commandBuffer;
}
