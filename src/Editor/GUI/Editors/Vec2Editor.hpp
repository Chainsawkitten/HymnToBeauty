#pragma once

#include "../Widget.hpp"

class Entity;
class Font;

namespace GUI {
    class Label;
    class FloatEditor;
    
    /// Used to edit a 2D vector.
    class Vec2Editor : public Widget {
        public:
            /// Create new vector editor.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            Vec2Editor(Widget* parent, Font* font);
            
            /// Destructor.
            ~Vec2Editor() override;
            
            /// Update the editor.
            void Update() override;
            
            /// Render the editor.
            void Render() override;
            
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
            
            /// Set which vec2 variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetVec2(glm::vec2* variable);
            
        private:
            glm::vec2 size;
            Font* font;
            
            glm::vec2* variable = nullptr;
            
            Label* xLabel;
            FloatEditor* xEditor;
            
            Label* yLabel;
            FloatEditor* yEditor;
    };
}
