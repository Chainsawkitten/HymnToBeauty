#pragma once

#include <string>
#include "../FileSelector.hpp"

class ScriptFile;
namespace GUI {
    /// Used to edit a sound.
    class ScriptEditor {
        public:
            /// Show the editor.
            void Show();
            
            /// Set the scriptfile to edit.
            /**
             * @param sound Scriptfile to edit.
             */
            void SetScript (ScriptFile* script);
            
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

			///The path of the script.
			//std::string* path;

        private:
            void FileSelected(const std::string& file);
            
            ScriptFile* script = nullptr;
            bool visible = false;
            
            FileSelector fileSelector;            

    };
}
