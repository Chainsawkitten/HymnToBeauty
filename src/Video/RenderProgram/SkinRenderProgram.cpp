#include "SkinRenderProgram.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../Geometry/Geometry3D.hpp"
#include "../Geometry/VertexType/SkinVertex.hpp"
#include "../Texture/Texture2D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "../LowLevelRenderer/Interface/Shader.hpp"
#include "../LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "../LowLevelRenderer/Interface/Buffer.hpp"
#include "../LowLevelRenderer/Interface/Texture.hpp"
#include <Utility/Log.hpp>

#include "Skinning.vert.hpp"
#include "Default3D.frag.hpp"
#include "DepthPrePassSkin.vert.hpp"
#include "DepthPrePass.frag.hpp"

using namespace Video;

SkinRenderProgram::SkinRenderProgram(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;

    vertexShader = lowLevelRenderer->CreateShader(SKINNING_VERT, Shader::Type::VERTEX_SHADER);
    fragmentShader = lowLevelRenderer->CreateShader(DEFAULT3D_FRAG, Shader::Type::FRAGMENT_SHADER);
    shaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, fragmentShader});

    // Create shaders for early rejection pass
    depthVertexShader = lowLevelRenderer->CreateShader(DEPTHPREPASSSKIN_VERT, Shader::Type::VERTEX_SHADER);
    depthFragmentShader = lowLevelRenderer->CreateShader(DEPTHPREPASS_FRAG, Shader::Type::FRAGMENT_SHADER);
    depthShaderProgram = lowLevelRenderer->CreateShaderProgram({depthVertexShader, depthFragmentShader});

    // Create graphics pipelines.
    vertexDescription = Geometry::VertexType::SkinVertex::GenerateVertexDescription(lowLevelRenderer);

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

    // Create uniform buffer.
    fragmentUniformBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(FragmentUniforms));
}

SkinRenderProgram::~SkinRenderProgram() {
    for (uint8_t i = 0; i < 2; ++i) {
        for (Buffer* matricesBuffer : matricesBuffers[i]) {
            delete matricesBuffer;
        }
    }
    delete fragmentUniformBuffer;

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

void SkinRenderProgram::PreDepthRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    SwapMatricesBuffers();

    commandBuffer.BindGraphicsPipeline(depthGraphicsPipeline);

    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    this->viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void SkinRenderProgram::DepthRender(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones) {
    commandBuffer.BindGeometry(geometry->GetGeometryBinding());

    // Matrices.
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::MATRICES, GetMatricesBuffer(bones));

    commandBuffer.PushConstants(&modelMatrix);

    commandBuffer.DrawIndexed(geometry->GetIndexCount());
}

void SkinRenderProgram::PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Buffer* lightBuffer, unsigned int lightCount) {
    commandBuffer.BindGraphicsPipeline(graphicsPipeline);
    this->viewMatrix = viewMatrix;
    this->projectionMatrix = projectionMatrix;
    this->viewProjectionMatrix = projectionMatrix * viewMatrix;

    // Fragment uniforms.
    FragmentUniforms fragmentUniforms;
    fragmentUniforms.lightCount = lightCount;
    fragmentUniforms.gamma = gamma;

    fragmentUniformBuffer->Write(&fragmentUniforms);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::FRAGMENT_UNIFORMS, fragmentUniformBuffer);

    // Light storage buffer.
    commandBuffer.BindStorageBuffer(lightBuffer);
}

void SkinRenderProgram::Render(CommandBuffer& commandBuffer, const Geometry::Geometry3D* geometry, const Texture2D* textureAlbedo, const Texture2D* textureNormal, const Texture2D* textureMetallic, const Texture2D* textureRoughness, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones) {
    commandBuffer.BindGeometry(geometry->GetGeometryBinding());

    // Textures.
    commandBuffer.BindMaterial({textureAlbedo->GetTexture(), textureNormal->GetTexture(), textureMetallic->GetTexture(), textureRoughness->GetTexture()});

    // Matrices.
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::MATRICES, GetMatricesBuffer(bones));

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

void SkinRenderProgram::SwapMatricesBuffers() {
    uint8_t otherFrame = 1 - frame;

    // Move all buffers to other frame.
    while (!matricesBuffers[otherFrame].empty()) {
        matricesBuffers[frame].push_front(matricesBuffers[otherFrame].front());
        matricesBuffers[otherFrame].pop_front();
    }

    frame = otherFrame;
}

Buffer* SkinRenderProgram::GetMatricesBuffer(const std::vector<glm::mat4>& bones) {
    uint8_t otherFrame = 1 - frame;

    // Reuse previously used buffer if one exists.
    Buffer* matricesBuffer = nullptr;
    if (!matricesBuffers[otherFrame].empty()) {
        matricesBuffer = matricesBuffers[otherFrame].front();
        matricesBuffers[otherFrame].pop_front();
    }

    // Otherwise, create a new one.
    if (matricesBuffer == nullptr) {
        matricesBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(MatricesValues));
        matricesBuffers[frame].push_front(matricesBuffer);
    }

    // Update buffer.
    MatricesValues matricesValues = {};
    matricesValues.viewProjectionMatrix = viewProjectionMatrix;
    matricesValues.viewMatrix = viewMatrix;
    for (std::size_t i = 0; i < bones.size(); ++i) {
        matricesValues.bones[i] = bones[i];
    }
    matricesBuffer->Write(&matricesValues);

    return matricesBuffer;
}
