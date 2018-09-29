#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

namespace Component {
    /// %Component describing a directional light source (like the sun).
    class DirectionalLight : public SuperComponent {
        public:
            /// Create new directional light.
            DirectionalLight();

            Json::Value Save() const override;

            /// Color.
            glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);

            /// Ambient coefficient.
            float ambientCoefficient = 0.f;
    };
}
