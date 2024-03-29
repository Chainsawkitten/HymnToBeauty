#include "ResourceManager.hpp"

#include <Video/Renderer.hpp>
#include <Video/Geometry/Rectangle.hpp>
#include "../Geometry/Cube.hpp"
#include "../Geometry/Model.hpp"
#include <Video/Texture/Texture2D.hpp>
#include "../Texture/TextureAsset.hpp"
#include "../Script/ScriptFile.hpp"
#include <Utility/Log.hpp>

using namespace std;

ResourceManager::ResourceManager(Video::Renderer* renderer) {
    this->lowLevelRenderer = renderer->GetLowLevelRenderer();

    // Create default textures for when no (valid) texture has been specified.
    defaultAlbedo = new TextureAsset(lowLevelRenderer, glm::uvec2(4, 4), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    defaultNormal = new TextureAsset(lowLevelRenderer, glm::uvec2(4, 4), glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
    defaultRoughnessMetallic = new TextureAsset(lowLevelRenderer, glm::uvec2(4, 4), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
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
