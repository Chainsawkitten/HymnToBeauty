#include "ResourceManager.hpp"

#include "../Animation/AnimationClip.hpp"
#include "../Animation/AnimationController.hpp"
#include "../Animation/Skeleton.hpp"
#include <Video/Geometry/Rectangle.hpp>
#include "../Geometry/Cube.hpp"
#include "../Geometry/Model.hpp"
#include <Video/Texture/TexturePNG.hpp>
#include "../Audio/SoundBuffer.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Script/ScriptFile.hpp"
#include <Utility/Log.hpp>
#include "../Audio/VorbisFile.hpp"

using namespace std;

Geometry::Model* ResourceManager::CreateModel(const std::string& name) {
    if (models.find(name) == models.end()) {
        Geometry::Model* model = new Geometry::Model();
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

Animation::AnimationClip* ResourceManager::CreateAnimationClip(const std::string& name) {
    if (animationClips.find(name) == animationClips.end()) {
        Animation::AnimationClip* animationClip = new Animation::AnimationClip();
        animationClip->Load(name);
        animationClips[name].animationClip = animationClip;
        animationClipsInverse[animationClip] = name;
        animationClips[name].count = 1;
    } else
        animationClips[name].count++;

    return animationClips[name].animationClip;
}

void ResourceManager::FreeAnimationClip(Animation::AnimationClip* animationClip) {
    std::string name = animationClipsInverse[animationClip];

    if (animationClips[name].count-- <= 1) {
        animationClipsInverse.erase(animationClip);
        delete animationClip;
        animationClips.erase(name);
    }
}

Animation::AnimationController* ResourceManager::CreateAnimationController(const std::string& name) {
    if (animationControllers.find(name) == animationControllers.end()) {
        Animation::AnimationController* animationController = new Animation::AnimationController();
        animationController->Load(name);
        animationControllers[name].animationController = animationController;
        animationControllersInverse[animationController] = name;
        animationControllers[name].count = 1;
    } else
        animationControllers[name].count++;

    return animationControllers[name].animationController;
}

void ResourceManager::FreeAnimationController(Animation::AnimationController* animationController) {
    std::string name = animationControllersInverse[animationController];

    if (animationControllers[name].count-- <= 1) {
        animationControllersInverse.erase(animationController);
        delete animationController;
        animationControllers.erase(name);
    }
}

Animation::Skeleton* ResourceManager::CreateSkeleton(const std::string& name) {
    if (skeletons.find(name) == skeletons.end()) {
        Animation::Skeleton* skeleton = new Animation::Skeleton;
        skeleton->Load(name);
        skeletons[name].skeleton = skeleton;
        skeletonsInverse[skeleton] = name;
        skeletons[name].count = 1;
    } else
        skeletons[name].count++;

    return skeletons[name].skeleton;
}

void ResourceManager::FreeSkeleton(Animation::Skeleton* skeleton) {
    std::string name = skeletonsInverse[skeleton];

    if (skeletons[name].count-- <= 1) {
        skeletonsInverse.erase(skeleton);
        delete skeleton;
        skeletons.erase(name);
    }
}

Video::TexturePNG* ResourceManager::CreateTexturePNG(const char* data, int dataLength) {
    if (textures.find(data) == textures.end()) {
        textures[data].texture = new Video::TexturePNG(data, dataLength);
        texturesInverse[textures[data].texture] = data;
        textures[data].count = 1;
    } else
        textures[data].count++;

    return textures[data].texture;
}

void ResourceManager::FreeTexturePNG(Video::TexturePNG* texture) {
    const char* data = texturesInverse[texture];

    if (textures[data].count-- <= 1) {
        texturesInverse.erase(texture);
        delete texture;
        textures.erase(data);
    }
}

TextureAsset* ResourceManager::CreateTextureAsset(const std::string& name) {
    if (textureAssets.find(name) == textureAssets.end()) {
        TextureAsset* textureAsset = new TextureAsset();
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
