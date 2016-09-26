#pragma once

#include <functional>
#include <map>
#include "Editors/TextureEditor.hpp"

class Texture2D;
class Entity;
namespace Geometry {
    class OBJModel;
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
            
            /// Set function to call when an entity has been selected.
            /**
             * @param callback Function to call.
             */
            void SetEntitySelectedCallback(std::function<void(Entity*)> callback);
            
            /// Set function to call when a model has been selected.
            /**
             * @param callback Function to call.
             */
            void SetModelSelectedCallback(std::function<void(Geometry::OBJModel*)> callback);
            
            /// Set function to call when a sound has been selected.
            /**
             * @param callback Function to call.
             */
            void SetSoundSelectedCallback(std::function<void(Audio::SoundBuffer*)> callback);
            
        private:
            bool visible = false;
            
            std::map<Texture2D*, TextureEditor> textureEditors;
            
            bool hasEntitySelectedCallback = false;
            std::function<void(Entity*)> entitySelectedCallback;
            
            bool hasModelSelectedCallback = false;
            std::function<void(Geometry::OBJModel*)> modelSelectedCallback;
            
            bool hasSoundSelectedCallback = false;
            std::function<void(Audio::SoundBuffer*)> soundSelectedCallback;
    };
}
