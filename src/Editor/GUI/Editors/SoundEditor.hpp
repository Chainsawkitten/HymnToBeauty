#pragma once

#include <string>
#include "../FileSelector.hpp"

namespace Audio {
    class SoundFile;
}

namespace GUI {
    /// Used to edit a sound.
    class SoundEditor {
        public:
            /// Constructor.
            SoundEditor();

            /// Show the editor.
            void Show();
            
            /// Get the sound being edited.
            /**
             * @return The sound being edited.
             */
            const Audio::SoundFile* GetSound() const;
            
            /// Set the sound to edit.
            /**
             * @param sound Sound to edit.
             */
            void SetSound(Audio::SoundFile* sound);
            
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
            
            Audio::SoundFile* sound = nullptr;
            bool visible = false;
            
            FileSelector fileSelector;
            
            char name[128];
    };
}
