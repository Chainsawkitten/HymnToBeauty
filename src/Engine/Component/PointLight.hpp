#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

namespace Component {
/// %Component describing a point light.
class PointLight : public SuperComponent {
  public:
    /// Create new point light.
    PointLight();

    void Serialize(Json::Value& node, bool load) override;

    /// Color.
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    /// Attenuation.
    float attenuation = 1.0f;

    /// Intensity.
    float intensity = 1.0f;

    /// Distance.
    float distance = 1.0f;
};
} // namespace Component
