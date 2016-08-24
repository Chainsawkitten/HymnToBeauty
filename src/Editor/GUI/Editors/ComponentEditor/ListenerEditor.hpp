#pragma once

#include "ComponentEditor.hpp"

class Entity;

namespace GUI {
    /// Used to edit a listener component.
    class ListenerEditor : public ComponentEditor {
        public:
            /// Create new listener editor.
            /**
             * @param parent Parent widget.
             */
            ListenerEditor(Widget* parent);
            
            /// Set the entity to edit the listener component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
    };
}
