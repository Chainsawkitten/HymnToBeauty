#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

namespace Component {
/// %Component describing a directional light source (like the sun).
class DirectionalLight : public SuperComponent {
  public:
    /// Create new directional light.
    DirectionalLight();

    void Serialize(Json::Value& node, bool load) override;

    /// Color.
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    /// Ambient coefficient.
    float ambientCoefficient = 0.0f;
};
} // namespace Component
