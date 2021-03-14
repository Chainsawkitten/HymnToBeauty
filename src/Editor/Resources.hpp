#pragma once

#include <vector>
#include <string>
#include <json/value.h>

class TextureAsset;
class ScriptFile;
namespace Geometry {
class Model;
}
namespace Audio {
class SoundFile;
}

namespace Json {
class Value;
}

/// A list of all the resources in a hymn.
class ResourceList {
    friend ResourceList& Resources();

  public:
    /// A resource.
    struct Resource {
        /// Get the name of the resource.
        /**
         * @return The name of the resource.
         */
        std::string GetName() const;

        /// The filename.
        std::string filename;

        /// The type of resource.
        enum Type { SCENE = 0, MODEL, TEXTURE, SOUND, SCRIPT, GLTF, INVALID } type;

        /// Scene name.
        std::string* scene;

        /// Model.
        Geometry::Model* model;

        /// Texture.
        TextureAsset* texture;

        /// Sound.
        Audio::SoundFile* sound;

        /// Script.
        ScriptFile* script;
    };

    /// A folder containing resources.
    struct ResourceFolder {
        /// The name of the folder.
        std::string name;

        /// Subfolders.
        std::vector<ResourceFolder> subfolders;

        /// The contained resources.
        std::vector<Resource> resources;
    };

    /// Save all resources to file.
    void Save() const;

    /// Creates a Json file for the resources.
    /**
     * @return A json file representing the resources.
     */
    Json::Value ToJson() const;

    /// Load all resources from file.
    void Load();

    /// Clear resources.
    void Clear();

    /// Resources.
    ResourceFolder resourceFolder;

    /// The name of the activeScene.
    std::string activeScene;

    /// The id of the next scene to create.
    unsigned int sceneNumber = 0U;

    /// The id of the next model to create.
    unsigned int modelNumber = 0U;

    /// The id of the next texture to create.
    unsigned int textureNumber = 0U;

    /// The id of the next sound to create.
    unsigned int soundNumber = 0U;

    /// The id of the next script to create.
    unsigned int scriptNumber = 0U;

    /// Gets the path where it's saved.
    /**
     * @return The full path.
     */
    std::string GetSavePath() const;

    /// Convert extension to type.
    /**
     * @param extension The file extension.
     * @return The type of resource.
     */
    static Resource::Type ExtensionToType(const std::string& extension);

  private:
    static ResourceList& GetInstance();

    ResourceFolder LoadFolder(const std::string& name, std::string path);
    void ClearFolder(ResourceFolder& folder);

    ResourceList();
    ResourceList(ResourceList const&) = delete;
    void operator=(ResourceList const&) = delete;
};

/// Get the resource list.
/**
 * @return The %ResourceList instance.
 */
ResourceList& Resources();
