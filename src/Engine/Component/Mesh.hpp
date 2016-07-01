#pragma once

#include "SuperComponent.hpp"

namespace Geometry {
    class Geometry3D;
}

namespace Component {
    /// %Component providing geometry to an entity.
    class Mesh : public SuperComponent {
        public:
            /// Create new mesh.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Mesh(Entity* entity);
            
            /// Destructor.
            ~Mesh() final;

            /// Geometry data.
            /**
             * Default: nullptr
             */
            Geometry::Geometry3D* geometry;
    };
}
