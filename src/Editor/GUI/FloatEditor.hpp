#pragma once

#include "TextField.hpp"

namespace GUI {
    /// Used to edit a float.
    class FloatEditor : public TextField {
        public:
            /// Create new float editor.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            FloatEditor(Widget* parent, Font* font);
            
            /// Set which float variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetFloat(float* variable);
            
        protected:
            /// Called when text has been updated.
            void TextUpdated() override;
            
        private:
            float* variable = nullptr;
    };
}
