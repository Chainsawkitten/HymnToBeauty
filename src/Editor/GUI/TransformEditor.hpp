#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Label;
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
            
            Label* transformLabel;
            
            Label* positionLabel;
            Vec3Editor* positionEditor;
            
            Label* scaleLabel;
            Vec3Editor* scaleEditor;
            
            Label* rotationLabel;
            Vec3Editor* rotationEditor;
    };
}
