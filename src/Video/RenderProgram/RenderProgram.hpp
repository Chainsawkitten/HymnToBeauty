#pragma once

#include <glm/glm.hpp>

namespace Video {
class Texture2D;
class ShaderProgram;
class StorageBuffer;
namespace Geometry {
class Geometry3D;
}

/// Super class render program.
class RenderProgram {
  public:
    /// Constructor.
    RenderProgram();

    /// Destructor.
    virtual ~RenderProgram();

    /// Gamma correction.
    /**
     * @param gamma Gamma to apply.
     */
    void SetGamma(float gamma);

    /// Gamma correction.
    /**
     * @return gamma Gamma value.
     */
    float GetGamma() const;

    /// Set whether fog is applied.
    /**
     * @param fogApply Whether to apply fog.
     */
    void SetFogApply(bool fogApply);

    /// Get whether fog is applied.
    /**
     * @return Whether fog is applied.
     */
    bool GetFogApply() const;

    /// Set fog density.
    /**
     * @param fogDensity Density of the fog.
     */
    void SetFogDensity(float fogDensity);

    /// get fog density.
    /**
     * @return Density of the fog
     */
    float GetFogDensity() const;

    /// Set fog color.
    /**
     * @param fogColor Color of the fog.
     */
    void SetFogColor(const glm::vec3& fogColor);

    /// Get fog color.
    /**
     * @return Color of the fog.
     */
    glm::vec3 GetFogColor() const;

    /// Set whether color filter is applied.
    /**
     * @param colorFilterApply Whether to apply color filter.
     */
    void SetColorFilterApply(bool colorFilterApply);

    /// Get whether color filter is applied.
    /**
     * @return Whether color filter is applied.
     */
    bool GetColorFilterApply() const;

    /// Set color filer color.
    /**
     * @param colorFilterColor Color of the color filter.
     */
    void SetColorFilterColor(const glm::vec3& colorFilterColor);

    /// Get color filer color.
    /**
     * @return Color of the color filter.
     */
    glm::vec3 GetColorFilterColor() const;

    /// Set whether dithering is applied.
    /**
     * @param ditherApply Whether to apply dithering.
     */
    void SetDitherApply(bool ditherApply);

    /// Get whether dithering is applied.
    /**
     * @return Whether dithering is applied.
     */
    bool GetDitherApply() const;

    /// Set size of the current frame to be rendered in pixels.
    /**
     * @param frameSize Size of the frame in pixels.
     */
    void SetFrameSize(const glm::vec2& frameSize);

    /// Get size of the current frame to be rendered in pixels.
    /**
     * @return Size of the frame in pixels.
     */
    glm::vec2 GetFrameSize() const;

  protected:
    float gamma = 2.2f;
    int fogApply = false;
    float fogDensity = 0.002f;
    glm::vec3 fogColor = glm::vec3(1.f, 0.f, 0.f);
    int colorFilterApply = false;
    glm::vec3 colorFilterColor = glm::vec3(0, 1, 0);
    int ditherApply = true;
    glm::vec2 frameSize = glm::vec2(800.f, 800.f);
};
} // namespace Video
