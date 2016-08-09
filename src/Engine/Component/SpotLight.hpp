#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

class Entity;

namespace Component {
    /// %Component describing a spot light.
    class SpotLight : public SuperComponent {
        public:
            /// Create new spot light.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            SpotLight(Entity* entity);
            
            /// Color.
            glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);

            /// Ambient coefficient.
            float ambientCoefficient = 0.f;

            /// Attenuation.
            float attenuation = 1.f;

            /// Intensity.
            float intensity = 1.f;
            
            /// Angle of the cone (in degrees).
            float coneAngle = 15.f;
    };
}
