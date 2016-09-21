#pragma once

#include "../TextField.hpp"

namespace GUI {
    /// Used to edit a double.
    class DoubleEditor : public TextField {
        public:
            /// Create new double editor.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            DoubleEditor(Widget* parent, Font* font);
            
            /// Set which double variable to edit.
            /**
             * @param variable The variable to edit.
             */
            void SetDouble(double* variable);
            
        protected:
            /// Called when text has been updated.
            void TextUpdated() override;
            
        private:
            double* variable = nullptr;
    };
}
