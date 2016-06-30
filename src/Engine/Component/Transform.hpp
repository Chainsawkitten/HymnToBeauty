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
            
            /// Position in the world.
            /**
             * Default: 0.f, 0.f, 0.f
             */
            glm::vec3 position;
            
            /// Scale.
            /**
             * Default: 1.f, 1.f, 1.f
             */
            glm::vec3 scale;
            
            /// Rotation (yaw, pitch, roll in degrees).
            /**
             * Default: 0.f, 0.f, 0.f
             */
            glm::vec3 rotation;
    };
}
