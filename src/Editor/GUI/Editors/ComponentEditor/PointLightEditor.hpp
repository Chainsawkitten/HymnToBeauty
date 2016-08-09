#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Vec3Editor;
    class FloatEditor;
    
    /// Used to edit a point light component.
    class PointLightEditor : public ComponentEditor {
        public:
            /// Create new point light editor.
            /**
             * @param parent Parent widget.
             */
            PointLightEditor(Widget* parent);
            
            /// Destructor.
            ~PointLightEditor() override;
            
            /// Set the entity to edit the point light component of.
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
    };
}
