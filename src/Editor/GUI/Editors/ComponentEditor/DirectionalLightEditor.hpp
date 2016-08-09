#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Vec3Editor;
    class FloatEditor;
    
    /// Used to edit a directional light component.
    class DirectionalLightEditor : public ComponentEditor {
        public:
            /// Create new directional light editor.
            /**
             * @param parent Parent widget.
             */
            DirectionalLightEditor(Widget* parent);
            
            /// Destructor.
            ~DirectionalLightEditor() override;
            
            /// Set the entity to edit the directional light component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            Font* font;
            
            Vec3Editor* colorEditor;
            FloatEditor* ambientCoefficientEditor;
    };
}
