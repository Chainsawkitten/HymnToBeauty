#pragma once

#include <functional>
#include <vector>
#include <string>

namespace GUI {
    /// A window where a hymn can be selected.
    class SelectHymnWindow {
        public:
            /// Constructor.
            SelectHymnWindow();

            /// Scan the save directory for hymns.
            void Scan();
            
            /// Show the window and let the user select a hymn.
            void Show();
            
            /// Set function to call when closed.
            /**
             * @param callback Function to call when window is closed.
             */
            void SetClosedCallback(const std::function<void(const std::string&)>& callback);
            
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
            
            /// Set window title.
            /**
             * @param title Window title.
             */
            void SetTitle(const char* title);
            
            /// Set the name of the open button.
            /**
             * @param openButtonName The name of the open button.
             */
            void SetOpenButtonName(const char* openButtonName);
            
        private:
            // Interaction
            bool hasClosedCallback = false;
            std::function<void(const std::string&)> closedCallback;
            
            std::vector<std::string> files;
            char name[128];
            
            bool visible = false;
            
            const char* title;
            const char* openButtonName;
    };
}
