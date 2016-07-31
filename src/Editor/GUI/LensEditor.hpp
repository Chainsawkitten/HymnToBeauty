#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Label;
    class FloatEditor;
    
    /// Used to edit a lens component.
    class LensEditor : public ComponentEditor {
        public:
            /// Create new transform editor.
            /**
             * @param parent Parent widget.
             */
            LensEditor(Widget* parent);
            
            /// Destructor.
            ~LensEditor() override;
            
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
            void SetEntity(Entity* entity) override;
            
        private:
            glm::vec2 size;
            Font* font;
            
            Entity* entity;
            
            Label* lensLabel;
            
            Label* fieldOfViewLabel;
            FloatEditor* fieldOfViewEditor;
            
            Label* zNearLabel;
            FloatEditor* zNearEditor;
            
            Label* zFarLabel;
            FloatEditor* zFarEditor;
    };
}
