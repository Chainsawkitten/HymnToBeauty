#pragma once

#include <string>
#include "EntityEditor.hpp"

class Entity;

namespace GUI {
    /// Used to edit a scene.
    class SceneEditor {
        public:
            /// Show the editor.
            void Show();
            
            /// Set the scene to edit.
            /**
             * @param scene Scene to edit.
             */
            void SetScene(std::size_t scene);
            
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
            
            /// Save currently open scene to file.
            void Save() const;

			///Did we press on an entity this frame.
			bool entityPressed = false;

			///The Entity Editor
			EntityEditor entityEditor;
            
        private:
            bool visible = false;
            
            std::size_t sceneIndex;
            char name[128] = "";
            
    };
}
