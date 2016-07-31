#pragma once

#include "Widget.hpp"

class Entity;

namespace GUI {
    /// Used to edit a component.
    class ComponentEditor : public Widget {
        public:
            /// Create new component editor.
            /**
             * @param parent Parent widget.
             */
            ComponentEditor(Widget* parent);
            
            /// Destructor.
            ~ComponentEditor() override;
            
            /// Set the entity to edit a component of.
            /**
             * @param entity %Entity to edit.
             */
            virtual void SetEntity(Entity* entity) = 0;
    };
}
