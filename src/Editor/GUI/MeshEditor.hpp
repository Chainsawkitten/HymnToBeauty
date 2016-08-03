#pragma once

#include "Widget.hpp"

class Font;
namespace Geometry {
    class Rectangle;
    class OBJModel;
}

namespace GUI {
    class Label;
    class StringEditor;
    class Button;
    
    /// Used to edit a mesh.
    class MeshEditor : public Widget {
        public:
            /// Create new mesh editor.
            /**
             * @param parent Parent widget.
             */
            MeshEditor(Widget* parent);
            
            /// Destructor.
            ~MeshEditor();
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position) override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set the mesh to edit.
            /**
             * @param mesh Mesh to edit.
             */
            void SetMesh(Geometry::OBJModel* mesh);
            
        private:
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Geometry::OBJModel* mesh;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Button* loadButton;
    };
}
