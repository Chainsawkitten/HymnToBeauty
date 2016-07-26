#pragma once

#include "Widget.hpp"

class Entity;
class Font;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    class FloatEditor;
    
    /// Used to edit a transform component.
    class TransformEditor : public Widget {
        public:
            /// Create new transform editor.
            /**
             * @param parent Parent widget.
             */
            TransformEditor(Widget* parent);
            
            /// Destructor.
            ~TransformEditor();
            
            /// Update the editor.
            void Update() override;
            
            /// Render the editor.
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
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set the entity to edit the transform component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity);
            
        private:
            glm::vec2 size;
            
            Entity* entity;
            
            Geometry::Rectangle* rectangle;
            
            Font* font;
            FloatEditor* xEditor;
    };
}
