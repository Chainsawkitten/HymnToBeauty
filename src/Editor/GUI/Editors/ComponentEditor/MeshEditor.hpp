#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;
namespace Component {
    class Mesh;
}
namespace Geometry {
    class OBJModel;
}

namespace GUI {
    class TextButton;
    class ModelSelector;
    
    /// Used to edit a mesh component.
    class MeshEditor : public ComponentEditor {
        public:
            /// Create new mesh editor.
            /**
             * @param parent Parent widget.
             * @param modelSelector Model selector to use.
             */
            MeshEditor(Widget* parent, ModelSelector* modelSelector);
            
            /// Destructor.
            ~MeshEditor() override;
            
            /// Set the entity to edit the mesh component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            void SelectModelPressed();
            void ModelSelected(Geometry::OBJModel* model);
            
            Font* font;
            
            Component::Mesh* mesh = nullptr;
            TextButton* selectModelButton;
            
            ModelSelector* modelSelector;
    };
}
