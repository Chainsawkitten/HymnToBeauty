#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>
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

/// Render program to render an entity using skin shader program.
class SkinRenderProgram : public RenderProgram {
  public:
    /// Create new skin render program.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit SkinRenderProgram(LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~SkinRenderProgram() final;

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
     * @param bones Transformations of skeleton.
     */
    void DepthRender(CommandBuffer& commandBuffer, Geometry::Geometry3D* geometry, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones);

    /// Bind render program.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param viewMatrix The camera's view matrix.
     * @param projectionMatrix The camera's projection matrix.
     * @param lightBuffer Storage buffer containing light data.
     * @param lightCount Number of lights in the light buffer.
     */
    void PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Buffer* lightBuffer, unsigned int lightCount);

    /// Render skinned geometry.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param geometry The geometry to render.
     * @param textureAlbedo Albedo texture.
     * @param normalTexture Normal texture.
     * @param textureMetallic Metallic texture.
     * @param textureRoughness Roughness texture.
     * @param modelMatrix Model matrix.
     * @param bones Transformations of skeleton.
     */
    void Render(CommandBuffer& commandBuffer, const Video::Geometry::Geometry3D* geometry, const Video::Texture2D* textureAlbedo, const Video::Texture2D* normalTexture, const Video::Texture2D* textureMetallic, const Video::Texture2D* textureRoughness, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones);

  private:
    SkinRenderProgram(const SkinRenderProgram& other) = delete;

    void SwapMatricesBuffers();
    Buffer* GetMatricesBuffer(const std::vector<glm::mat4>& bones);
    
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

    static const int maxBones = 100;
    struct MatricesValues {
        glm::mat4 viewProjectionMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 bones[maxBones];
    };

    std::list<Buffer*> matricesBuffers[2];
    uint8_t frame = 0;

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
