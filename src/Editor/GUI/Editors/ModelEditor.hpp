#pragma once

#include <string>
#include <glm/glm.hpp>
#include "../FileSelector.hpp"
#include "../../Resources.hpp"

namespace Geometry {
    class Model;
}

namespace GUI {
    /// Used to edit a model.
    class ModelEditor {
        public:
            /// Constructor.
            ModelEditor();

            /// Show the editor.
            void Show();

            /// Get the model being edited.
            /**
             * @return The model being edited.
             */
            const Geometry::Model* GetModel() const;

            /// Set the model to edit.
            /**
             * @param folder Resource folder containing the model.
             * @param model Model to edit.
             */
            void SetModel(ResourceList::ResourceFolder* folder, Geometry::Model* model);

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
            void FileSelected(const std::string& file);
            void RefreshImportSettings();
            TextureAsset* LoadTexture(const std::string& path, const std::string& name);

            ResourceList::ResourceFolder* folder = nullptr;
            Geometry::Model* model = nullptr;
            bool visible = false;

            FileSelector fileSelector;

            char name[128];

            std::string source;
            std::string destination;
            std::string msgString;

            bool hasSourceFile = false;
            bool isImported = false;
            bool uniformScaling = false;
            glm::vec3 scale = glm::vec3(1.0f);
            bool triangulate = true;
            bool importNormals = true;
            bool importTangents = true;
            bool importTextures = false;
            bool bindPose = false;
            bool flipUVs = false;
            bool createScene = false;
            bool CPU = false;
            bool GPU = true;
    };
}
