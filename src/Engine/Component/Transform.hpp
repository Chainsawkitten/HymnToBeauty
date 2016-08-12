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
            
            /// Get the model matrix.
            /**
             * @return The model matrix.
             */
            glm::mat4 GetModelMatrix() const;
            
            /// Get orientation matrix.
            /**
             * @return The entity's orientation matrix.
             */
            glm::mat4 GetOrientation() const;
            
            /// Get orientation matrix (for camera).
            /**
             * Calculates the orientation matrix as if the entity was a camera.
             * @return The entity's orientation matrix.
             */
            glm::mat4 GetCameraOrientation() const;
            
            /// Get direction of the entity.
            /**
             * @return The entity's direction.
             */
            glm::vec3 GetDirection() const;
            
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
