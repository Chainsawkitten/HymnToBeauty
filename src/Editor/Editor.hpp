#pragma once

#include "GUI/SelectHymnWindow.hpp"
#include "GUI/ResourceList.hpp"
#include <string>

/// Handles the main editor.
class Editor {
    public:
        /// Create new editor.
        Editor();
        
        /// Show the editor.
        void Show();
        
        /// Save the hymn being edited.
        void Save() const;
        
        /// Get whether the resource list is visible.
        /**
         * @return Whether the resource list is visible.
         */
        bool IsVisible() const;
        
        /// Set whether the resource list should be visible.
        /**
         * @param visible Whether the resource list should be visible.
         */
        void SetVisible(bool visible);
        
    private:
        void Play();
        void NewHymn();
        void NewHymnClosed(const std::string& hymn);
        void OpenHymn();
        void OpenHymnClosed(const std::string& hymn);
        
        bool visible = true;
        GUI::SelectHymnWindow selectHymnWindow;
        GUI::ResourceList resourceList;
};
