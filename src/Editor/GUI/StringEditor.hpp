#pragma once

#include "TextField.hpp"

namespace GUI {
    /// Used to edit a string.
    class StringEditor : public TextField {
        public:
            /// Create new string editor.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            StringEditor(Widget* parent, Font* font);
            
            /// Set which string variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetString(std::string* variable);
            
        protected:
            /// Called when text has been updated.
            void TextUpdated() override;
            
        private:
            std::string* variable;
    };
}
