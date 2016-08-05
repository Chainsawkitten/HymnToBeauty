#pragma once

#include "Widget.hpp"

class Entity;
class Font;

namespace GUI {
    class Label;
    class FloatEditor;
    
    /// Used to edit a 3D vector.
    class Vec3Editor : public Widget {
        public:
            /// Create new vector editor.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            Vec3Editor(Widget* parent, Font* font);
            
            /// Destructor.
            ~Vec3Editor() override;
            
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
            
            /// Set which vec3 variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetVec3(glm::vec3* variable);
            
        private:
            glm::vec2 size;
            Font* font;
            
            glm::vec3* variable = nullptr;
            
            Label* xLabel;
            FloatEditor* xEditor;
            
            Label* yLabel;
            FloatEditor* yEditor;
            
            Label* zLabel;
            FloatEditor* zEditor;
    };
}
