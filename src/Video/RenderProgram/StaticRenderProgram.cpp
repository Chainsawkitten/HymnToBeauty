#include "StaticRenderProgram.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../Geometry/Geometry3D.hpp"
#include "../Geometry/VertexType/StaticVertex.hpp"
#include "../Texture/Texture2D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "../LowLevelRenderer/Interface/Shader.hpp"
#include "../LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "../LowLevelRenderer/Interface/Texture.hpp"
#include "../LowLevelRenderer/Interface/Buffer.hpp"

#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"
#include "DepthPrePassStatic.vert.hpp"
#include "DepthPrePass.frag.hpp"

using namespace Video;

StaticRenderProgram::StaticRenderProgram(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;

    vertexShader = lowLevelRenderer->CreateShader(DEFAULT3D_VERT, Shader::Type::VERTEX_SHADER);
    fragmentShader = lowLevelRenderer->CreateShader(DEFAULT3D_FRAG, Shader::Type::FRAGMENT_SHADER);
    shaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, fragmentShader});

    // Create shaders for early rejection pass
    depthVertexShader = lowLevelRenderer->CreateShader(DEPTHPREPASSSTATIC_VERT, Shader::Type::VERTEX_SHADER);
    depthFragmentShader = lowLevelRenderer->CreateShader(DEPTHPREPASS_FRAG, Shader::Type::FRAGMENT_SHADER);
    depthShaderProgram = lowLevelRenderer->CreateShaderProgram({depthVertexShader, depthFragmentShader});

    // Create graphics pipelines.
    vertexDescription = Geometry::VertexType::StaticVertex::GenerateVertexDescription(lowLevelRenderer);

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::BACK;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::TEST;
    configuration.depthComparison = DepthComparison::LESS_EQUAL;

    graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);

    configuration.depthComparison = DepthComparison::LESS;
    configuration.depthMode = DepthMode::TEST_WRITE;
    depthGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(depthShaderProgram, configuration, vertexDescription);

    sampler = lowLevelRenderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::REPEAT);
}

StaticRenderProgram::~StaticRenderProgram() {
    delete graphicsPipeline;
    delete depthGraphicsPipeline;

    delete shaderProgram;
    delete depthShaderProgram;

    delete vertexShader;
    delete fragmentShader;
    delete depthVertexShader;
    delete depthFragmentShader;

    delete vertexDescription;
}

void StaticRenderProgram::PreDepthRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    commandBuffer.BindGraphicsPipeline(depthGraphicsPipeline);

    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    // Matrices.
    Buffer* depthMatricesBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::mat4), &viewProjectionMatrix);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::MATRICES, depthMatricesBuffer);
}

void StaticRenderProgram::DepthRender(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, const glm::mat4& modelMatrix) const {
    commandBuffer.BindGeometry(geometry->GetGeometryBinding());
    commandBuffer.PushConstants(&modelMatrix);
    commandBuffer.DrawIndexed(geometry->GetIndexCount());
}

void StaticRenderProgram::PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const ZBinning::LightInfo& lightInfo) {
    commandBuffer.BindGraphicsPipeline(graphicsPipeline);
    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    // Matrices.
    MatricesValues matricesValues;
    matricesValues.viewProjectionMatrix = viewProjectionMatrix;
    matricesValues.viewMatrix = viewMatrix;

    Buffer* matricesBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(MatricesValues), &matricesValues);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matricesBuffer);

    // Fragment uniforms.
    FragmentUniforms fragmentUniforms;
    fragmentUniforms.directionalLightCount = lightInfo.directionalLightCount;
    fragmentUniforms.lightCount = lightInfo.lightCount;
    fragmentUniforms.maskCount = lightInfo.maskCount;
    fragmentUniforms.zBins = lightInfo.zBins;
    fragmentUniforms.tileSize = lightInfo.tileSize;
    fragmentUniforms.tilesX = lightInfo.tiles.x;
    fragmentUniforms.gamma = gamma;
    fragmentUniforms.zNear = lightInfo.zNear;
    fragmentUniforms.zFar = lightInfo.zFar;

    Buffer* fragmentUniformBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(FragmentUniforms), &fragmentUniforms);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, fragmentUniformBuffer);

    // Light storage buffer.
    commandBuffer.BindStorageBuffers({ lightInfo.directionalLightBuffer, lightInfo.lightBuffer, lightInfo.zMaskBuffer, lightInfo.tileMaskBuffer });
}

void StaticRenderProgram::Render(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, Video::Texture2D* textureAlbedo, Video::Texture2D* textureNormal, Video::Texture2D* textureRoughnessMetallic, const glm::mat4& modelMatrix) const {
    commandBuffer.BindGeometry(geometry->GetGeometryBinding());

    // Textures
    commandBuffer.BindMaterial({
        {textureAlbedo->GetTexture(), sampler},
        {textureNormal->GetTexture(), sampler},
        {textureRoughnessMetallic->GetTexture(), sampler}
    });

    // Render model.
    struct PushConstants {
        glm::mat4 modelMatrix;
        glm::mat4 normalMatrix;
    } pushConstants;

    pushConstants.modelMatrix = modelMatrix;
    pushConstants.normalMatrix = glm::transpose(glm::inverse(viewMatrix * modelMatrix));

    commandBuffer.PushConstants(&pushConstants);

    commandBuffer.DrawIndexed(geometry->GetIndexCount());
}
