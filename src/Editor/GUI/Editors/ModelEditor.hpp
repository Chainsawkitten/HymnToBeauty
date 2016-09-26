#pragma once

#include <string>
#include "../FileSelector.hpp"

namespace Geometry {
    class OBJModel;
}

namespace GUI {
    /// Used to edit a model.
    class ModelEditor {
        public:
            /// Show the editor.
            void Show();
            
            /// Set the model to edit.
            /**
             * @param model Model to edit.
             */
            void SetModel(Geometry::OBJModel* model);
            
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
            
            Geometry::OBJModel* model = nullptr;
            bool visible = false;
            
            FileSelector fileSelector;
            
            char name[128] = "";
    };
}
