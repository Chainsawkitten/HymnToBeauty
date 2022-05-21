#include "SpriteRenderProgram.hpp"

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
#include "../LowLevelRenderer/Interface/GeometryBinding.hpp"

#include "Sprite.vert.hpp"
#include "Sprite.frag.hpp"

using namespace Video;

SpriteRenderProgram::SpriteRenderProgram(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;

    // Create quad geometry.
    glm::vec2 vertex[6];
    vertex[0] = glm::vec2(0.0, 1.0);
    vertex[1] = glm::vec2(0.0, 0.0);
    vertex[2] = glm::vec2(1.0, 1.0);
    vertex[3] = glm::vec2(0.0, 0.0);
    vertex[4] = glm::vec2(1.0, 0.0);
    vertex[5] = glm::vec2(1.0, 1.0);

    vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, sizeof(glm::vec2) * 6, &vertex);

    VertexDescription::Attribute attribute;
    attribute.size = 2;
    attribute.type = VertexDescription::AttributeType::FLOAT;
    attribute.normalized = false;

    vertexDescription = lowLevelRenderer->CreateVertexDescription(1, &attribute);
    geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer);

    // Create graphics pipeline.
    vertexShader = lowLevelRenderer->CreateShader(SPRITE_VERT, Shader::Type::VERTEX_SHADER);
    fragmentShader = lowLevelRenderer->CreateShader(SPRITE_FRAG, Shader::Type::FRAGMENT_SHADER);
    shaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, fragmentShader});

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA;
    configuration.depthMode = DepthMode::TEST;
    configuration.depthComparison = DepthComparison::LESS_EQUAL;

    graphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(shaderProgram, configuration, vertexDescription);
}

SpriteRenderProgram::~SpriteRenderProgram() {
    delete graphicsPipeline;

    delete shaderProgram;

    delete vertexShader;
    delete fragmentShader;

    delete geometryBinding;
    delete vertexDescription;
    delete vertexBuffer;
}

void SpriteRenderProgram::PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewProjectionMatrix) {
    commandBuffer.BindGraphicsPipeline(graphicsPipeline);
    commandBuffer.BindGeometry(geometryBinding);

    // Matrices.
    MatricesValues matricesValues;
    matricesValues.viewProjectionMatrix = viewProjectionMatrix;

    Buffer* matricesBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(MatricesValues), &matricesValues);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matricesBuffer);

    // Fragment uniforms.
    FragmentUniforms fragmentUniforms;
    fragmentUniforms.gamma = gamma;

    Buffer* fragmentUniformBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(FragmentUniforms), &fragmentUniforms);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, fragmentUniformBuffer);
}

void SpriteRenderProgram::Render(CommandBuffer& commandBuffer, Video::Texture2D* texture, const glm::vec2& size, const glm::vec2& pivot, const glm::mat4& modelMatrix, const glm::vec4& tint) const {
    // Textures
    commandBuffer.BindMaterial({texture->GetTexture()});

    // Render sprite.
    struct PushConstants {
        glm::mat4 modelMatrix;
        glm::vec4 sizePivot;
        glm::vec4 tint;
    } pushConstants;

    pushConstants.modelMatrix = modelMatrix;
    pushConstants.sizePivot = glm::vec4(size.x, size.y, pivot.x, pivot.y);
    pushConstants.tint = tint;

    commandBuffer.PushConstants(&pushConstants);

    commandBuffer.Draw(6);
}
