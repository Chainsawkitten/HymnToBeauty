#pragma once

#include <string>
#include <map>
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
            void SetScene(std::string* scene);
            
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
            
        private:
            void ShowEntity(Entity* entity);
            
            bool visible = false;
            
            std::string* scene = nullptr;
            char name[128] = "";
            
            std::map<Entity*, EntityEditor> entityEditors;
    };
}
