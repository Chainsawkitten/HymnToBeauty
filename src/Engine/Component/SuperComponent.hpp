#pragma once

class Entity;

namespace Component {
    /// %Component which all components inherit.
    class SuperComponent {
        public:
            /// Create new %SuperComponent.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            SuperComponent(Entity* entity);
            
            /// Destructor.
            virtual ~SuperComponent();
            
            /// Pointer to which entity this component corresponds.
            Entity* entity;
    };
}
