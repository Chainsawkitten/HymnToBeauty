#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

namespace Component {
/// %Component describing a point light.
class PointLight : public SuperComponent {
  public:
    /// Create new point light.
    PointLight();

    Json::Value Save() const override;

    /// Color.
    glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);

    /// Attenuation.
    float attenuation = 1.f;

    /// Intensity.
    float intensity = 1.f;

    /// Distance.
    float distance = 1.f;
};
} // namespace Component
