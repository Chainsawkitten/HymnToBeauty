#pragma once

#include <string>
#include <vector>
#include "Scene/Scene.hpp"

class Texture2D;
namespace Geometry {
    class Model;
}
namespace Audio {
    class SoundBuffer;
}

/// A hymn to beauty.
class ActiveHymn {
    friend ActiveHymn& Hymn();
    
    public:
        /// Clear the hymn of all properties.
        void Clear();
        
        /// Get the path where the hymn is saved.
        /**
         * @return The hymn's path.
         */
        const std::string& GetPath() const;
        
        /// Set the path where the hymn is saved.
        /**
         * @param path New path.
         */
        void SetPath(const std::string& path);
        
        /// Save the hymn.
        void Save() const;
        
        /// Load a hymn.
        /**
         * @param path Path to the saved hymn.
         */
        void Load(const std::string& path);
        
        /// Update the scene.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(float deltaTime);
        
        /// Render the active scene.
        /**
         * @param soundSources Whether to show sound sources.
         * @param particleEmitters Whether to show particle emitters.
         * @param lightSources Whether to show light sources.
         */
        void Render(bool soundSources = false, bool particleEmitters = false, bool lightSources = false);
        
        /// The active scene.
        Scene activeScene;
        
        /// The id of the next entity to create.
        unsigned int entityNumber = 1U;
        
        /// Models.
        std::vector<Geometry::Model*> models;
        
        /// The id of the next model to create.
        unsigned int modelNumber = 0U;
        
        /// Textures.
        std::vector<Texture2D*> textures;
        
        /// The id of the next texture to create.
        unsigned int textureNumber = 0U;
        
        /// Sounds.
        std::vector<Audio::SoundBuffer*> sounds;
        
        /// The id of the next sound to create.
        unsigned int soundNumber = 0U;
        
        /// Default diffuse texture.
        Texture2D* defaultDiffuse;
        
        /// Default normal texture.
        Texture2D* defaultNormal;
        
        /// Default specular texture.
        Texture2D* defaultSpecular;
        
        /// Default glow texture.
        Texture2D* defaultGlow;
        
    private:
        static ActiveHymn& GetInstance();
        
        ActiveHymn();
        ActiveHymn(ActiveHymn const&) = delete;
        void operator=(ActiveHymn const&) = delete;
        
        std::string path;
};

/// Get the active hymn.
/**
 * @return The %ActiveHymn instance.
 */
ActiveHymn& Hymn();
