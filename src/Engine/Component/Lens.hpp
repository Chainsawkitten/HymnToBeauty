#pragma once

#include "SuperComponent.hpp"
#include <glm\glm.hpp>

namespace Component {
    /// %Component handling a camera lens.
    class Lens : public SuperComponent {
        public:
            /// Create new lens.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Lens(Entity* entity);
            
            /// Destructor.
            ~Lens() final;
            
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

            /// Get the projection matrix.
            /**
             * @param screenSize Screen size in pixels.
             * @return The projection matrix.
             */
            glm::mat4 GetProjection(const glm::vec2& screenSize) const;
            
            /// Field of view, in degrees.
            /**
             * Default: 45.0
             */
            float fieldOfView = 45.f;
            
            /// Near plane.
            /**
             * Default: 0.5
             */
            float zNear = 0.5f;
            
            /// Far plane.
            /**
             * Default: 100.0
             */
            float zFar = 100.f;
    };
}
