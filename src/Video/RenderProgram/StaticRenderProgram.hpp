#pragma once

#include <glm/glm.hpp>
#include "RenderProgram.hpp"

namespace Video {
class LowLevelRenderer;
class Texture2D;
class Shader;
class ShaderProgram;
class GraphicsPipeline;
class Buffer;
class CommandBuffer;
class Texture;
class VertexDescription;
namespace Geometry {
class Geometry3D;
}

/// Render program to render an entity using default shader program.
class StaticRenderProgram : public RenderProgram {
  public:
    /// Create new static render program.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit StaticRenderProgram(LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~StaticRenderProgram();

    /// Bind depth render program.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param viewMatrix The camera's view matrix.
     * @param projectionMatrix The camera's projection matrix.
     */
    void PreDepthRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /// Render depth pass.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param geometry The geometry to render.
     * @param modelMatrix Model matrix.
     */
    void DepthRender(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, const glm::mat4& modelMatrix) const;

    /// Bind render program.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param viewMatrix The camera's view matrix.
     * @param projectionMatrix The camera's projection matrix.
     * @param lightBuffer Storage buffer containing light data.
     * @param lightCount Number of lights in the light buffer.
     */
    void PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Buffer* lightBuffer, unsigned int lightCount);

    /// Render mesh.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param geometry The geometry to render.
     * @param textureAlbedo Albedo texture.
     * @param textureNormal Normal texture.
     * @param textureMetallic Metallic texture.
     * @param textureRoughness Roughness texture.
     * @param modelMatrix Model matrix.
     */
    void Render(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, const Video::Texture2D* textureAlbedo, const Video::Texture2D* textureNormal, const Video::Texture2D* textureMetallic, const Video::Texture2D* textureRoughness, const glm::mat4& modelMatrix) const;

  private:
    StaticRenderProgram(const StaticRenderProgram& other) = delete;

    VertexDescription* vertexDescription;

    Shader* depthVertexShader;
    Shader* depthFragmentShader;
    ShaderProgram* depthShaderProgram;
    GraphicsPipeline* depthGraphicsPipeline;

    Buffer* depthMatricesBuffer;

    Shader* vertexShader;
    Shader* fragmentShader;
    ShaderProgram* shaderProgram;
    GraphicsPipeline* graphicsPipeline;

    struct MatricesValues {
        glm::mat4 viewProjectionMatrix;
        glm::mat4 viewMatrix;
    };
    Buffer* matricesBuffer;

    struct FragmentUniforms {
        int32_t lightCount;
        float gamma;
    };
    Buffer* fragmentUniformBuffer;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;
};
} // namespace Video
