#pragma once

#include <map>
#include "Editors/ModelEditor.hpp"
#include "Editors/SceneEditor.hpp"
#include "Editors/SoundEditor.hpp"
#include "Editors/TextureEditor.hpp"

class Texture2D;
namespace Geometry {
    class Model;
}
namespace Audio {
    class SoundBuffer;
}

namespace GUI {
    /// Displays all the hymn's resources.
    class ResourceList {
        public:
            /// Show the resource list.
            void Show();
            
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
            
            /// Hide all editors.
            /**
             * Needs to be called before playing the game or old editors with stale pointers could be shown when returning to the editor.
             */
            void HideEditors();
            
            /// Save the currently active scene.
            void SaveScene() const;
            
            /// Reset which scene is open.
            void ResetScene();
            
        private:
            bool visible = false;
            
            SceneEditor sceneEditor;
            std::map<Geometry::Model*, ModelEditor> modelEditors;
            std::map<Texture2D*, TextureEditor> textureEditors;
            std::map<Audio::SoundBuffer*, SoundEditor> soundEditors;
    };
}
