#pragma once

#include "SuperComponent.hpp"
#include "../Geometry/Model.hpp"

namespace Geometry {
    class Model;
}

namespace Component {
    /// %Component handling animations.
    class Animation : public SuperComponent {
        public:
            /// Create new %Animation component.
            Animation();
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Rigged model to animate.
            /**
             * Default: nullptr
             */
            Geometry::Model* riggedModel = nullptr;

            /// Total time of animation.
            /**
             * Default: 0
             */
            float time = 0.f;

    };
}
