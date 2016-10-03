#pragma once

#include <functional>
#include <vector>

namespace GUI {
    /// A window where a file can be selected.
    class FileSelector {
        public:
            /// Create new window.
            FileSelector();
            
            /// Show the file selector.
            void Show();
            
            /// Set function to call when a file has been selected.
            /**
             * @param callback Function to call.
             */
            void SetFileSelectedCallback(std::function<void(const std::string&)> callback);
            
            /// Set extension to select files of.
            /**
             * @param extension Set the extension of which files to show.
             */
            void SetExtension(const std::string& extension);
            
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
            void OpenParentDirectory();
            void OpenDirectory(const std::string& name);
            void SelectFile(const std::string& name);
            void ScanDirectory();
            
            bool visible = false;
            
            // Interaction
            std::string path;
            std::string extension = "";
            bool pathChanged = true;
            
            std::vector<std::string> directories;
            std::vector<std::string> files;
            
            bool hasFileSelectedCallback = false;
            std::function<void(const std::string&)> fileSelectedCallback;
    };
}
