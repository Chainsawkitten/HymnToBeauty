#pragma once

namespace GUI {
    /// Used to edit Hymn to Beauty settings.
    class SettingsWindow {
        public:
            /// Show the settings window.
            void Show();
            
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
            bool visible = false;
    };
}
