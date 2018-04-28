#pragma once

#include <string>
#include "EntityEditor.hpp"
#include "../ResourceSelector.hpp"

class Entity;

namespace GUI {
    /// Used to edit a scene.
    class SceneEditor {
        public:
            /// Constructor.
            SceneEditor();

            /// Show the editor.
            void Show();

            /// Set the scene to edit.
            /**
             * @param path Path to the folder containing the scene.
             * @param scene Scene to edit.
             */
            void SetScene(const std::string& path, std::string* scene);

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

            /// Gets a Json file representing the save file.
            /**
             * @param filename This is where the filename will be stored.
             */
            Json::Value GetSaveFileJson(std::string* filename) const;

            /// Did we press on an entity this frame.
            bool entityPressed = false;

            /// The Entity Editor
            EntityEditor entityEditor;

        private:
            void ShowEntity(Entity* entity);
            Entity* draggedEntity = nullptr;

            bool visible = false;

            std::string path;
            std::string* scene = nullptr;

            char name[128];

            ResourceSelector sceneSelector;
    };
}
