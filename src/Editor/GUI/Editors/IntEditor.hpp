#pragma once

#include "../TextField.hpp"

namespace GUI {
    /// Used to edit an integer.
    class IntEditor : public TextField {
        public:
            /// Create new int editor.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            IntEditor(Widget* parent, Font* font);
            
            /// Set which integer variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetInt(int* variable);
            
        protected:
            /// Called when text has been updated.
            void TextUpdated() override;
            
        private:
            int* variable = nullptr;
    };
}
