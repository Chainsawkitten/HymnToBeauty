#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

namespace Component {
    /// %Component providing transformation.
    class Transform : public SuperComponent {
        public:
            /// Create new %Transform component.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Transform(Entity* entity);
            
            /// Destructor.
            ~Transform() final;
            
            /// Get the model matrix.
            /**
             * @return The model matrix.
             */
            glm::mat4 GetModelMatrix() const;
            
            /// Get orientation matrix (for camera).
            /**
             * Calculates the orientation matrix as if the entity was a camera.
             * @return The entity's orientation matrix.
             */
            glm::mat4 GetCameraOrientation() const;
            
            /// Position in the world.
            /**
             * Default: 0.f, 0.f, 0.f
             */
            glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
            
            /// Scale.
            /**
             * Default: 1.f, 1.f, 1.f
             */
            glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
            
            /// Rotation (yaw, pitch, roll in degrees).
            /**
             * Default: 0.f, 0.f, 0.f
             */
            glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f);
    };
}
