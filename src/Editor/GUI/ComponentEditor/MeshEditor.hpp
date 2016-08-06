#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;
namespace Component {
    class Mesh;
}

namespace GUI {
    class TextButton;
    
    /// Used to edit a mesh component.
    class MeshEditor : public ComponentEditor {
        public:
            /// Create new mesh editor.
            /**
             * @param parent Parent widget.
             */
            MeshEditor(Widget* parent);
            
            /// Destructor.
            ~MeshEditor() override;
            
            /// Set the entity to edit the mesh component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            void SelectModelPressed();
            
            Font* font;
            
            Component::Mesh* mesh = nullptr;
            TextButton* selectModelButton;
    };
}
