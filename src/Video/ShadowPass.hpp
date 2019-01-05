#pragma once

/// Handles writing and creation of shadowmap.
namespace Video {
class ShadowPass {
  public:
    /// Create new default shadow program.
    ShadowPass();

    /// Removes GL buffers.
    ~ShadowPass();

    /// Create shadow buffer for writing.
    void InitDepthMap();

    /// Bind Texture to depth buffer.
    void BindBuffer();

    /// Set the size of the shadow map.
    /**
     * @param shadowMapSize The size of the shadow map.
     */
    void SetShadowMapSize(unsigned int shadowMapSize);

    /// Get the size of the shadow map.
    /**
     * @return The size of the shadow map.
     */
    unsigned int GetShadowMapSize() const;

    /// Get shadow map framebuffer object.
    /**
     * @return Shadow map framebuffer object.
     */
    int GetDepthMapFbo();

    /// Get shadow map texture id.
    /**
     * @return Shadow map texture id.
     */
    int GetShadowID();

  private:
    // Depth map to save the texture.
    unsigned int depthMap;

    // Depth map framebuffer object.
    unsigned int depthMapFbo;

    // Size of the shadow map.
    unsigned int shadowSize = 1024;
};
} // namespace Video
