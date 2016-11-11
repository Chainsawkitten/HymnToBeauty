#pragma once

#include "SuperComponent.hpp"
#include "../Geometry/RiggedModel.hpp"

namespace Component {
    /// %Component handling animations.
    class Animation : public SuperComponent {
        public:
            /// Create new %Animation component.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Animation(Entity* entity);
            
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
            
            /// Rigged model to animate.
            /**
             * Default: nullptr
             */
            Geometry::RiggedModel* riggedModel = nullptr;

            /// Total time of animation.
            /**
             * Default: 0
             */
            float time = 0.f;
    };
}
