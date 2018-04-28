#pragma once

#include <string>

class ScriptFile;

namespace GUI {
    /// Used to edit a script.
    class ScriptEditor {
        public:
            /// Show the editor.
            void Show();

            /// Get the script file being edited
            /**
             * @return The script file being edited.
             */
            const ScriptFile* GetScript() const;

            /// Set the script file to edit.
            /**
             * @param script %Script file to edit.
             */
            void SetScript(ScriptFile* script);

            /// Get whether the window is visible.
            /**
             * @return Whether the window is visible.
             */
            bool IsVisible() const;

            /// Set whether the window should be visible.
            /**
             * @param visible Whether the window should be visible.
             */
            void SetVisible(bool visible);

        private:
            enum Status {
                UNKNOWN,
                SUCCESS,
                FAILURE
            } status = UNKNOWN;

            ScriptFile* script = nullptr;
            bool visible = false;
    };
}
