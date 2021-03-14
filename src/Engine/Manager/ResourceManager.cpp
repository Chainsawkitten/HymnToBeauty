#include "ResourceManager.hpp"

#include <Video/Renderer.hpp>
#include <Video/Geometry/Rectangle.hpp>
#include "../Geometry/Cube.hpp"
#include "../Geometry/Model.hpp"
#include <Video/Texture/Texture2D.hpp>
#include "../Audio/SoundBuffer.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Script/ScriptFile.hpp"
#include <Utility/Log.hpp>
#include "../Audio/VorbisFile.hpp"

#include "DefaultAlbedo.png.hpp"
#include "DefaultNormal.png.hpp"
#include "DefaultRoughnessMetallic.png.hpp"

using namespace std;

ResourceManager::ResourceManager(Video::Renderer* renderer) {
    this->lowLevelRenderer = renderer->GetLowLevelRenderer();

    defaultAlbedo = new TextureAsset(lowLevelRenderer, DEFAULTALBEDO_PNG, DEFAULTALBEDO_PNG_LENGTH);
    defaultNormal = new TextureAsset(lowLevelRenderer, DEFAULTNORMAL_PNG, DEFAULTNORMAL_PNG_LENGTH);
    defaultRoughnessMetallic = new TextureAsset(lowLevelRenderer, DEFAULTROUGHNESSMETALLIC_PNG, DEFAULTROUGHNESSMETALLIC_PNG_LENGTH);
}

ResourceManager::~ResourceManager() {
    delete defaultAlbedo;
    delete defaultNormal;
    delete defaultRoughnessMetallic;
}

Geometry::Model* ResourceManager::CreateModel(const std::string& name) {
    if (models.find(name) == models.end()) {
        Geometry::Model* model = new Geometry::Model(lowLevelRenderer);
        model->Load(name);
        models[name].model = model;
        modelsInverse[model] = name;
        models[name].count = 1;
    } else
        models[name].count++;

    return models[name].model;
}

void ResourceManager::FreeModel(Geometry::Model* model) {
    string name = modelsInverse[model];

    if (models[name].count-- <= 1) {
        modelsInverse.erase(model);
        delete model;
        models.erase(name);
    }
}

Video::Texture2D* ResourceManager::CreateTexture2D(const char* data, int dataLength) {
    if (textures.find(data) == textures.end()) {
        textures[data].texture = new Video::Texture2D(lowLevelRenderer, data, dataLength);
        texturesInverse[textures[data].texture] = data;
        textures[data].count = 1;
    } else
        textures[data].count++;

    return textures[data].texture;
}

void ResourceManager::FreeTexture2D(Video::Texture2D* texture) {
    const char* data = texturesInverse[texture];

    if (textures[data].count-- <= 1) {
        texturesInverse.erase(texture);
        delete texture;
        textures.erase(data);
    }
}

TextureAsset* ResourceManager::CreateTextureAsset(const std::string& name) {
    if (textureAssets.find(name) == textureAssets.end()) {
        TextureAsset* textureAsset = new TextureAsset(lowLevelRenderer);
        textureAsset->Load(name);
        textureAssets[name].textureAsset = textureAsset;
        textureAssetsInverse[textureAsset] = name;
        textureAssets[name].count = 1;
    } else
        textureAssets[name].count++;

    return textureAssets[name].textureAsset;
}

void ResourceManager::FreeTextureAsset(TextureAsset* textureAsset) {
    std::string name = textureAssetsInverse[textureAsset];

    if (textureAssets[name].count-- <= 1) {
        textureAssetsInverse.erase(textureAsset);
        delete textureAsset;
        textureAssets.erase(name);
    }
}

int ResourceManager::GetTextureAssetInstanceCount(TextureAsset* textureAsset) {
    std::string name = textureAssetsInverse[textureAsset];
    return textureAssets[name].count;
}

Audio::SoundFile* ResourceManager::CreateSound(const string& name) {
    if (sounds.find(name) == sounds.end()) {
        Audio::SoundFile* soundFile = new Audio::VorbisFile();
        soundFile->Load(name);
        sounds[name].sound = soundFile;
        soundsInverse[soundFile] = name;
        sounds[name].count = 1;
    } else
        sounds[name].count++;

    return sounds[name].sound;
}

void ResourceManager::FreeSound(Audio::SoundFile* soundFile) {
    string name = soundsInverse[soundFile];

    if (sounds[name].count-- <= 1) {
        soundsInverse.erase(soundFile);
        delete soundFile;
        sounds.erase(name);
    }
}

ScriptFile* ResourceManager::CreateScriptFile(const string& name) {
    if (scriptFiles.find(name) == scriptFiles.end()) {
        ScriptFile* scriptFile = new ScriptFile();
        scriptFile->Load(name);
        scriptFiles[name].scriptFile = scriptFile;
        scriptFilesInverse[scriptFile] = name;
        scriptFiles[name].count = 1;
    } else
        scriptFiles[name].count++;

    return scriptFiles[name].scriptFile;
}

void ResourceManager::FreeScriptFile(ScriptFile* scriptFile) {
    string name = scriptFilesInverse[scriptFile];

    if (scriptFiles[name].count-- <= 1) {
        scriptFilesInverse.erase(scriptFile);
        delete scriptFile;
        scriptFiles.erase(name);
    }
}

TextureAsset* ResourceManager::GetDefaultAlbedo() {
    return defaultAlbedo;
}

TextureAsset* ResourceManager::GetDefaultNormal() {
    return defaultNormal;
}

TextureAsset* ResourceManager::GetDefaultRoughnessMetallic() {
    return defaultRoughnessMetallic;
}
