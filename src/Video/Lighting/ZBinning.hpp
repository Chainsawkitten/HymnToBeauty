#pragma once

#include "Light.hpp"
#include "Isocahedron.hpp"

#include <vector>
#include <glm/glm.hpp>

namespace Video {

class LowLevelRenderer;
class Buffer;
class CommandBuffer;
class ComputePipeline;
class GraphicsPipeline;
class Shader;
class ShaderProgram;

/// Handles binning and tiling of lights.
class ZBinning {
  public:
    /// Information about the lights in a scene.
    struct LightInfo {
        /// The number of directional lights.
        uint32_t directionalLightCount;

        /// Buffer containing the directional light data.
        Buffer* directionalLightBuffer;

        /// The number of spot and point lights.
        uint32_t lightCount;

        /// Buffer containing the spot and point light data.
        Buffer* lightBuffer;

        /// The number of bitmasks per z-bin/tile.
        uint32_t maskCount;

        /// The size of a tile in pixels. Tiles are always square.
        uint32_t tileSize;

        /// The number of tiles in each dimension.
        glm::uvec2 tiles;

        /// Buffer containing light masks for the tiles.
        Buffer* tileMaskBuffer;

        /// The number of z-bins.
        uint32_t zBins;

        /// Buffer containing light masks for the z bins.
        Buffer* zMaskBuffer;

        /// The near plane.
        float zNear;

        /// The far plane.
        float zFar;
    };

    /// Create new Z binning.
    /**
     * @param lowLevelRenderer The low-level renderer.
     * @param screenSize The size of the screen in pixels.
     */
    ZBinning(LowLevelRenderer* lowLevelRenderer, const glm::uvec2& screenSize);

    /// Destructor.
    ~ZBinning();

    /// Configure tiles based on screen size.
    /**
     * @param size The size of the render surface.
     */
    void SetRenderSurfaceSize(const glm::uvec2& size);

    /// Bin and tile the lights.
    /**
     * @param commandBuffer Command buffer to record GPU work into.
     * @param directionalLights Vector of directional lights to push to the directional light buffer.
     * @param lights Vector of lights to push to the light buffer.
     * @param projectionMatrix Projection matrix.
     * @param zNear Near field.
     * @param zFar Far field.
     */
    void BinLights(CommandBuffer& commandBuffer, const std::vector<DirectionalLight>& directionalLights, const std::vector<Light>& lights, const glm::mat4& projectionMatrix, float zNear, float zFar);

    /// Get the light info.
    /**
     * @return The light info.
     */
    const LightInfo& GetLightInfo();

  private:
    void ClearBuffers(CommandBuffer& commandBuffer);
    void Binning(CommandBuffer& commandBuffer);
    void Tiling(CommandBuffer& commandBuffer, const glm::mat4& projectionMatrix);

    LowLevelRenderer* lowLevelRenderer;

    LightInfo lightInfo;
    bool conservativeRasterization;

    static const uint32_t MAX_DIRECTIONAL_LIGHTS = 128;
    DirectionalLight directionalLightData[MAX_DIRECTIONAL_LIGHTS];

    static const uint32_t MAX_LIGHTS = 2048;
    Light lightData[MAX_LIGHTS];

	static const uint32_t TILE_MASK_BUFFER_SIZE = 8 * 1024 * 1024;

    Shader* binningShader;
    ShaderProgram* binningShaderProgram;
    ComputePipeline* binningPipeline;

    Shader* tilingVertexShader;
    Shader* tilingFragmentShader;
    ShaderProgram* tilingShaderProgram;
    GraphicsPipeline* tilingPipeline;
    Isocahedron isocahedron;

    struct TilingMatricesData {
        glm::mat4 projectionMatrix;
        glm::vec2 scale;
        float invIsocahedronError;
    };

    struct TilingUniformData {
        uint32_t tileSize;
        uint32_t maskCount;
        uint32_t tilesX;
    };

    glm::uvec2 screenSize;
    glm::uvec2 tilingSize;

    uint32_t msaa = 1u;
    uint32_t msaaScale = 1u;
};

} // namespace Video
