#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Vec3Editor;
    
    /// Used to edit a transform component.
    class TransformEditor : public ComponentEditor {
        public:
            /// Create new transform editor.
            /**
             * @param parent Parent widget.
             */
            TransformEditor(Widget* parent);
            
            /// Destructor.
            ~TransformEditor() override;
            
            /// Set the entity to edit the transform component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            Font* font;
            
            Vec3Editor* positionEditor;
            Vec3Editor* scaleEditor;
            Vec3Editor* rotationEditor;
    };
}
