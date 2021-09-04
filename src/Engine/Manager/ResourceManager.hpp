#pragma once

#include <map>
#include <string>

namespace Video {
class Renderer;
class LowLevelRenderer;
class Texture2D;
namespace Geometry {
class Rectangle;
}
} // namespace Video
namespace Geometry {
class Cube;
class Model;
} // namespace Geometry
namespace Audio {
class SoundFile;
class AudioMaterial;
} // namespace Audio

class TextureAsset;
class ScriptFile;

/// Handles all resources.
class ResourceManager {
    friend class Hub;

  public:
    /// Constructor
    /**
     * @param renderer The renderer.
     */
    explicit ResourceManager(Video::Renderer* renderer);

    /// Destructor.
    ~ResourceManager();

    /// Create a model if it doesn't already exist.
    /**
     * @param name Name of model.
     * @return The model instance
     */
    Geometry::Model* CreateModel(const std::string& name);

    /// Free the reference to the model.
    /**
     * @param model %Model to dereference.
     */
    void FreeModel(Geometry::Model* model);

    /// Create a 2D texture if it doesn't already exist.
    /**
     * @param data Image file data.
     * @param dataLength Length of the image file data.
     * @return The %Texture2D instance
     */
    Video::Texture2D* CreateTexture2D(const char* data, int dataLength);

    /// Free the reference to the 2D texture.
    /**
     * Deletes the instance if no more references exist.
     * @param texture %Texture to dereference.
     */
    void FreeTexture2D(Video::Texture2D* texture);

    /// Create a texture asset if it doesn't already exist.
    /**
     * @param name The name of the texture asset.
     * @return The %TextureAsset instance
     */
    TextureAsset* CreateTextureAsset(const std::string& name);

    /// Free the reference to the texture asset.
    /**
     * Deletes the instance if no more references exist.
     * @param textureAsset %TextureAsset to dereference.
     */
    void FreeTextureAsset(TextureAsset* textureAsset);

    /// Get the number of instances of a texture asset.
    /**
     * @param textureAsset The texture asset to check.
     * @return How many instances of the texture asset currently exist.
     */
    int GetTextureAssetInstanceCount(TextureAsset* textureAsset);

    /// Create a sound if it doesn't already exist.
    /**
     * @param name Name of the sound.
     * @return The %SoundBuffer instance.
     */
    Audio::SoundFile* CreateSound(const std::string& name);

    /// Free the reference to the sound.
    /**
     * Deletes the instance if no more references exist.
     * @param soundFile %SoundFile to dereference.
     */
    void FreeSound(Audio::SoundFile* soundFile);

    /// Create a script file if it doesn't already exist.
    /**
     * @param name Name of the script file.
     * @return The %ScriptFile instance.
     */
    ScriptFile* CreateScriptFile(const std::string& name);

    /// Free the reference to the script file.
    /**
     * Deletes the instance if no more references exist.
     * @param scriptFile %ScriptFile to dereference.
     */
    void FreeScriptFile(ScriptFile* scriptFile);

    /// Get the default albedo texture.
    /**
     * @return The default albedo texture.
     */
    TextureAsset* GetDefaultAlbedo();

    /// Get the default normal map.
    /**
     * @return The default normal map.
     */
    TextureAsset* GetDefaultNormal();

    /// Get the default roughness-metallic texture.
    /**
     * @return The default roughness-metallic texture.
     */
    TextureAsset* GetDefaultRoughnessMetallic();

  private:
    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&) = delete;

    Video::LowLevelRenderer* lowLevelRenderer;

    // Rectangle
    Video::Geometry::Rectangle* rectangle = nullptr;
    int rectangleCount = 0;

    // Cube
    Geometry::Cube* cube = nullptr;

    int cubeCount = 0;

    // Model.
    struct ModelInstance {
        Geometry::Model* model;
        int count;
    };
    std::map<std::string, ModelInstance> models;
    std::map<Geometry::Model*, std::string> modelsInverse;

    // Texture2D.
    struct Texture2DInstance {
        Video::Texture2D* texture;
        int count;
    };
    std::map<const char*, Texture2DInstance> textures;
    std::map<Video::Texture2D*, const char*> texturesInverse;

    // Texture asset.
    struct TextureAssetInstance {
        TextureAsset* textureAsset;
        int count;
    };
    std::map<std::string, TextureAssetInstance> textureAssets;
    std::map<TextureAsset*, std::string> textureAssetsInverse;

    // Sound.
    struct SoundInstance {
        Audio::SoundFile* sound;
        int count;
    };
    std::map<std::string, SoundInstance> sounds;
    std::map<Audio::SoundFile*, std::string> soundsInverse;

    // ScriptFile.
    struct ScriptFileInstance {
        ScriptFile* scriptFile;
        int count;
    };
    std::map<std::string, ScriptFileInstance> scriptFiles;
    std::map<ScriptFile*, std::string> scriptFilesInverse;

    TextureAsset* defaultAlbedo;
    TextureAsset* defaultNormal;
    TextureAsset* defaultRoughnessMetallic;
};
