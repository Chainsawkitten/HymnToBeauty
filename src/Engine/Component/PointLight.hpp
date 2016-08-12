#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

class Entity;

namespace Component {
    /// %Component describing a point light.
    class PointLight : public SuperComponent {
        public:
            /// Create new point light.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            PointLight(Entity* entity);
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Load component from JSON node.
            /**
             * @param node JSON node to load from.
             */
            void Load(const Json::Value& node) override;
            
            /// Color.
            glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
            
            /// Ambient coefficient.
            float ambientCoefficient = 0.f;
            
            /// Attenuation.
            float attenuation = 1.f;

            /// Intensity.
            float intensity = 1.f;
    };
}
