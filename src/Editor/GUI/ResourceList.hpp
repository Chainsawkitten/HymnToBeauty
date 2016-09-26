#pragma once

#include <functional>
#include <map>
#include "Editors/ModelEditor.hpp"
#include "Editors/TextureEditor.hpp"
#include "Editors/SoundEditor.hpp"

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
            
        private:
            bool visible = false;
            
            std::map<Geometry::OBJModel*, ModelEditor> modelEditors;
            std::map<Texture2D*, TextureEditor> textureEditors;
            std::map<Audio::SoundBuffer*, SoundEditor> soundEditors;
            
            bool hasEntitySelectedCallback = false;
            std::function<void(Entity*)> entitySelectedCallback;
    };
}
