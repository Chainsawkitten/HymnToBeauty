#pragma once

#include <functional>
#include <string>

namespace GUI {
    /// A window that asks the user for the name of the folder to create.
    class FolderNameWindow {
        public:
            /// Default constructor.
            FolderNameWindow() = default;

            /// Show the window and let the user enter a name.
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

            /// Set function to call when closed.
            /**
             * @param callback Function to call when window is closed.
             */
            void SetClosedCallback(const std::function<void(const std::string&)>& callback);

        private:
            std::function<void(const std::string&)> closedCallback;

            bool visible = false;
            char name[128] = { '\0' };
    };
}
