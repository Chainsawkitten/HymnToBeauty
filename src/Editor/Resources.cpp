#include "Resources.hpp"

#include <fstream>
#include <Engine/Texture/TextureAsset.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Audio/SoundFile.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/FileSystem.hpp>

using namespace std;

string ResourceList::Resource::GetName() const {
    switch (type) {
    case Type::SCENE:
        return *scene;
    case Type::MODEL:
        return model->name;
    case Type::TEXTURE:
        return texture->name;
    case Type::SOUND:
        return sound->name;
    case Type::SCRIPT:
        return script->name;
    case Type::GLTF:
        return filename;
    default:
        return "";
    }
}

ResourceList::ResourceList() {
    resourceFolder.name = "Resources";
}

ResourceList& ResourceList::GetInstance() {
    static ResourceList resourceList;

    return resourceList;
}

void ResourceList::Save() const {
    // Save to file.
    ofstream file(GetSavePath());
    file << ToJson();
    file.close();
}

Json::Value ResourceList::ToJson() const {
    Json::Value root;

    root["activeScene"] = activeScene;
    root["sceneNumber"] = sceneNumber;
    root["modelNumber"] = modelNumber;
    root["textureNumber"] = textureNumber;
    root["soundNumber"] = soundNumber;
    root["scriptNumber"] = scriptNumber;

    return root;
}

void ResourceList::Load() {
    // Load Json document from file.
    Json::Value root;
    ifstream file(GetSavePath());
    file >> root;
    file.close();

    activeScene = root["activeScene"].asString();
    resourceFolder = LoadFolder("Resources", "");
    sceneNumber = root["sceneNumber"].asUInt();
    modelNumber = root["modelNumber"].asUInt();
    textureNumber = root["textureNumber"].asUInt();
    soundNumber = root["soundNumber"].asUInt();
    scriptNumber = root["scriptNumber"].asUInt();
}

void ResourceList::Clear() {
    ClearFolder(resourceFolder);
    resourceFolder.name = "Resources";

    sceneNumber = 0U;
    modelNumber = 0U;
    textureNumber = 0U;
    soundNumber = 0U;
    scriptNumber = 0U;
}

void ResourceList::Refresh() {
    ClearFolder(resourceFolder);
    resourceFolder = LoadFolder("Resources", "");
}

ResourceList::ResourceFolder ResourceList::LoadFolder(const string& name, std::string path) {
    ResourceFolder folder;
    folder.name = name;
    path += folder.name + "/";

    // Load subfolders.
    std::vector<std::string> subfolders = FileSystem::DirectoryContents(Hymn().GetPath() + FileSystem::DELIMITER + path, FileSystem::DIRECTORY);
    for (const std::string& dir : subfolders) {
        folder.subfolders.push_back(LoadFolder(dir, path));
    }

    // Load resources.
    std::vector<std::string> files = FileSystem::DirectoryContents(Hymn().GetPath() + FileSystem::DELIMITER + path, FileSystem::FILE);
    for (const std::string& file : files) {
        Resource resource;
        resource.filename = file;
        std::string extension = file.substr(file.find_last_of(".") + 1);
        resource.type = ExtensionToType(extension);

        switch (resource.type) {
        case Resource::SCENE:
            resource.scene = new std::string(file);
            break;
        case Resource::MODEL:
            resource.model = Managers().resourceManager->CreateModel(path + file);
            break;
        case Resource::TEXTURE:
            resource.texture = Managers().resourceManager->CreateTextureAsset(path + file);
            break;
        case Resource::SOUND:
            resource.sound = Managers().resourceManager->CreateSound(path + file);
            break;
        case Resource::SCRIPT:
            resource.script = Managers().resourceManager->CreateScriptFile(path + file);
            break;
        default:
            break;
        }

        if (resource.type != Resource::INVALID)
            folder.resources.push_back(resource);
    }

    return folder;
}

void ResourceList::ClearFolder(ResourceFolder& folder) {
    // Clear subfolders.
    for (ResourceFolder& subfolder : folder.subfolders)
        ClearFolder(subfolder);

    folder.subfolders.clear();

    // Clear resources.
    for (const Resource& resource : folder.resources) {
        switch (resource.type) {
        case Resource::Type::MODEL:
            Managers().resourceManager->FreeModel(resource.model);
            break;
        case Resource::Type::TEXTURE:
            Managers().resourceManager->FreeTextureAsset(resource.texture);
            break;
        case Resource::Type::SOUND:
            Managers().resourceManager->FreeSound(resource.sound);
            break;
        case Resource::Type::SCENE:
            delete resource.scene;
            break;
        default:
            break;
        }
    }

    folder.resources.clear();
}

std::string ResourceList::GetSavePath() const {
    return Hymn().GetPath() + FileSystem::DELIMITER + "Resources.json";
}

ResourceList::Resource::Type ResourceList::ExtensionToType(const std::string& extension) {
    if (extension == "png" || extension == "jpg" || extension == "jpeg") {
        return Resource::TEXTURE;
    } else if (extension == "mesh") {
        return Resource::MODEL;
    } else if (extension == "scn") {
        return Resource::SCENE;
    } else if (extension == "as") {
        return Resource::SCRIPT;
    } else if (extension == "ogg") {
        return Resource::SOUND;
    } else if (extension == "gltf") {
        return Resource::GLTF;
    }

    return Resource::INVALID;
}

ResourceList& Resources() {
    return ResourceList::GetInstance();
}
