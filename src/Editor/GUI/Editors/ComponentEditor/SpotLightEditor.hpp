#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Vec3Editor;
    class FloatEditor;
    
    /// Used to edit a spot light component.
    class SpotLightEditor : public ComponentEditor {
        public:
            /// Create new spot light editor.
            /**
             * @param parent Parent widget.
             */
            SpotLightEditor(Widget* parent);
            
            /// Destructor.
            ~SpotLightEditor() override;
            
            /// Set the entity to edit the spot light component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            Font* font;
            
            Vec3Editor* colorEditor;
            FloatEditor* ambientCoefficientEditor;
            FloatEditor* attenuationEditor;
            FloatEditor* intensityEditor;
            FloatEditor* coneAngleEditor;
    };
}
