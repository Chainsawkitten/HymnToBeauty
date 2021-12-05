#pragma once

#include <glm/glm.hpp>
#include "RenderProgram.hpp"
#include "../Lighting/ZBinning.hpp"

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
     * @param lightInfo Information about the lights in the scene.
     */
    void PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const ZBinning::LightInfo& lightInfo);

    /// Render mesh.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param geometry The geometry to render.
     * @param textureAlbedo Albedo texture.
     * @param textureNormal Normal texture.
     * @param textureRoughnessMetallic Roughness-metallic texture.
     * @param modelMatrix Model matrix.
     */
    void Render(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, Video::Texture2D* textureAlbedo, Video::Texture2D* textureNormal, Video::Texture2D* textureRoughnessMetallic, const glm::mat4& modelMatrix) const;

  private:
    StaticRenderProgram(const StaticRenderProgram& other) = delete;

    LowLevelRenderer* lowLevelRenderer;
    VertexDescription* vertexDescription;

    Shader* depthVertexShader;
    Shader* depthFragmentShader;
    ShaderProgram* depthShaderProgram;
    GraphicsPipeline* depthGraphicsPipeline;

    Shader* vertexShader;
    Shader* fragmentShader;
    ShaderProgram* shaderProgram;
    GraphicsPipeline* graphicsPipeline;

    struct MatricesValues {
        glm::mat4 viewProjectionMatrix;
        glm::mat4 viewMatrix;
    };

    struct FragmentUniforms {
        uint32_t directionalLightCount;
        uint32_t lightCount;
        uint32_t maskCount;
        uint32_t zBins;
        uint32_t tileSize;
        uint32_t tilesX;
        float gamma;
        float zNear;
        float zFar;
    };

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;
};
} // namespace Video
