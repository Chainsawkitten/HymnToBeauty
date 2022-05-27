#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

namespace Component {
/// %Component handling a camera through which the world can be rendered.
class Camera : public SuperComponent {
  public:
    /// Create new camera.
    Camera();

    Json::Value Save() const override;

    /// Get the projection matrix.
    /**
     * @param screenSize Screen size in pixels.
     * @return The projection matrix.
     */
    glm::mat4 GetProjection(const glm::vec2& screenSize) const;

    /// Whether the camera should use an orthographic projection, otherwise perspective projection.
    bool orthographic = false;

    /// Field of view, in degrees.
    /**
     * Default: 45.0
     * Only used for perspective projection.
     */
    float fieldOfView = 45.f;

    /// The size of the view volume.
    /**
     * Default: 10.0
     * Only used for orthographic projection.
     */
    float size = 10.0f;

    /// Near plane.
    /**
     * Default: 0.1
     */
    float zNear = 0.1f;

    /// Far plane.
    /**
     * Default: 100.0
     */
    float zFar = 100.f;

	/// Viewport.
	glm::vec4 viewport = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    /// Filter settings.
    struct FilterSettings {
        /// Whether to enable FXAA.
        bool fxaa = true;

        /// Whether to enable dithering.
        bool ditherApply = true;

        /// Gamma correction value.
        float gamma = 2.2f;

        /// Whether to enable bloom.
        bool bloom = false;

        /// Intensity of the bloom effect.
        float bloomIntensity = 1.0f;

        /// Threshold for the bloom effect.
        float bloomThreshold = 1.0f;

        /// How scattered the bloom effect should be.
        float bloomScatter = 0.7f;
    };

    /// Filter settings.
    FilterSettings filterSettings;
};
} // namespace Component
