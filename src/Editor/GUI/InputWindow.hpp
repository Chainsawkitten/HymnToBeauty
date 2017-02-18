#pragma once

#include <functional>
#include <string>

namespace GUI {
    /// A window where input is configured.
    class InputWindow {
        public:
            /// Show the window
            void Show();
            
            /// Set function to call when closed.
            /**
             * @param callback Function to call when window is closed.
             */
            void SetClosedCallback(std::function<void(const std::string&)> callback);
            
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
            void AddButton();
            
            // Interaction
            bool hasClosedCallback = false;
            std::function<void(const std::string&)> closedCallback;
            
            bool visible = false;
    };
}
