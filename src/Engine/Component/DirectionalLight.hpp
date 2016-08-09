#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

class Entity;

namespace Component {
    /// %Component describing a directional light source (like the sun).
    class DirectionalLight : public SuperComponent {
        public:
            /// Create new directional light.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            DirectionalLight(Entity* entity);
            
            /// Color.
            glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
            
            /// Ambient coefficient.
            float ambientCoefficient = 0.f;
    };
}
