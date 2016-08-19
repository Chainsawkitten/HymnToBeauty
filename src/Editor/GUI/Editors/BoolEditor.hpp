#pragma once

#include "../Widget.hpp"

class Texture2D;

namespace GUI {
    /// Used to edit a bool.
    class BoolEditor : public Widget {
        public:
            /// Create new bool editor.
            /**
             * @param parent Parent widget.
             */
            BoolEditor(Widget* parent);
            
            /// Destructor.
            ~BoolEditor() override;
            
            /// Set which bool variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetBool(bool* variable);
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            void Render() override;
            
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
            
        private:
            bool* variable = nullptr;
            
            Texture2D* uncheckedTexture;
            Texture2D* checkedTexture;
    };
}
