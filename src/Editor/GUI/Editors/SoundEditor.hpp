#pragma once

#include <string>
#include "../FileSelector.hpp"

namespace Audio {
    class SoundBuffer;
}

namespace GUI {
    /// Used to edit a sound.
    class SoundEditor {
        public:
            /// Show the editor.
            void Show();
            
            /// Set the sound to edit.
            /**
             * @param sound Sound to edit.
             */
            void SetSound(Audio::SoundBuffer* sound);
            
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
            void LoadPressed();
            void FileSelected(const std::string& file);
            
            Audio::SoundBuffer* sound = nullptr;
            bool visible = false;
            
            FileSelector fileSelector;
            
            char name[128] = "";
    };
}
