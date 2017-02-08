#include "Hymn.hpp"

#include "Util/FileSystem.hpp"
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Manager/ResourceManager.hpp"
#include "Manager/PhysicsManager.hpp"
#include "Manager/ParticleManager.hpp"
#include "Manager/ScriptManager.hpp"
#include "Manager/SoundManager.hpp"
#include "Manager/DebugDrawingManager.hpp"
#include "DefaultDiffuse.png.hpp"
#include "DefaultNormal.png.hpp"
#include "DefaultSpecular.png.hpp"
#include "DefaultGlow.png.hpp"
#include "Entity/Entity.hpp"
#include "Geometry/RiggedModel.hpp"
#include "Geometry/StaticModel.hpp"
#include "Texture/Texture2D.hpp"
#include "Audio/SoundBuffer.hpp"
#include <json/json.h>
#include <fstream>
#include "Util/Profiling.hpp"

#include "Component/Animation.hpp"

using namespace std;

ActiveHymn::ActiveHymn() {
    defaultDiffuse = Managers().resourceManager->CreateTexture2D(DEFAULTDIFFUSE_PNG, DEFAULTDIFFUSE_PNG_LENGTH, true);
    defaultNormal = Managers().resourceManager->CreateTexture2D(DEFAULTNORMAL_PNG, DEFAULTNORMAL_PNG_LENGTH);
    defaultSpecular = Managers().resourceManager->CreateTexture2D(DEFAULTSPECULAR_PNG, DEFAULTSPECULAR_PNG_LENGTH);
    defaultGlow = Managers().resourceManager->CreateTexture2D(DEFAULTGLOW_PNG, DEFAULTGLOW_PNG_LENGTH);
    
    Clear();
}

ActiveHymn& ActiveHymn::GetInstance() {
    static ActiveHymn ActiveHymn;
    
    return ActiveHymn;
}

void ActiveHymn::Clear() {
    path = "";
    world.Clear();
    
    entityNumber = 1U;
    
    for (Geometry::Model* model : models) {
        delete model;
    }
    models.clear();
    modelNumber = 0U;
    
    for (Texture2D* texture : textures) {
        delete texture;
    }
    textures.clear();
    textureNumber = 0U;
    
    for (Audio::SoundBuffer* sound : sounds) {
        delete sound;
    }
    sounds.clear();
    soundNumber = 0U;
}

const string& ActiveHymn::GetPath() const {
    return path;
}

void ActiveHymn::SetPath(const string& path) {
    this->path = path;
    FileSystem::CreateDirectory(path.c_str());
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Models").c_str());
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Scenes").c_str());
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Scripts").c_str());
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Sounds").c_str());
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Textures").c_str());
}

void ActiveHymn::Save() const {
    Json::Value root;
    
    // Save textures.
    Json::Value texturesNode;
    for (Texture2D* texture : textures) {
        texturesNode.append(texture->Save());
    }
    root["textures"] = texturesNode;
    
    // Save models.
    Json::Value modelsNode;
    for (Geometry::Model* model : models) {
        modelsNode.append(model->Save());
    }
    root["models"] = modelsNode;
    
    // Save sounds.
    Json::Value soundsNode;
    for (Audio::SoundBuffer* sound : sounds) {
        soundsNode.append(sound->Save());
    }
    root["sounds"] = soundsNode;
    
    // Save entities.
    Json::Value entitiesNode;
    for (Entity* entity : world.GetEntities()) {
        entitiesNode.append(entity->Save());
    }
    root["entities"] = entitiesNode;
    
    // Save to file.
    ofstream file(path + FileSystem::DELIMITER + "Hymn.json");
    file << root;
    file.close();
}

void ActiveHymn::Load(const string& path) {
    Clear();
    this->path = path;
    
    // Load Json document from file.
    Json::Value root;
    ifstream file(path + FileSystem::DELIMITER + "Hymn.json");
    file >> root;
    file.close();
    
    // Load textures.
    const Json::Value texturesNode = root["textures"];
    for (unsigned int i=0; i < texturesNode.size(); ++i) {
        Texture2D* texture = new Texture2D();
        texture->Load(texturesNode[i]);
        textures.push_back(texture);
    }
    
    // Load models.
    const Json::Value modelsNode = root["models"];
    for (unsigned int i=0; i < modelsNode.size(); ++i) {
        Geometry::Model* model;
        std::string type = modelsNode[i].get("type", "").asString();
        if (type == "Static") {
            model = new Geometry::StaticModel();
        } else {
            model = new Geometry::RiggedModel();
        }
        model->Load(modelsNode[i]);
        models.push_back(model);
    }
    
    // Load sounds.
    const Json::Value soundsNode = root["sounds"];
    for (unsigned int i=0; i < soundsNode.size(); ++i) {
        Audio::SoundBuffer* sound = new Audio::SoundBuffer();
        sound->Load(soundsNode[i]);
        sounds.push_back(sound);
    }
    
    // Load entities.
    const Json::Value entitiesNode = root["entities"];
    for (unsigned int i=0; i < entitiesNode.size(); ++i) {
        Entity* entity = world.CreateEntity("");
        entity->Load(entitiesNode[i]);
    }
}

void ActiveHymn::Update(float deltaTime) {
    { PROFILE("Run scripts.");
        Managers().scriptManager->Update(world);
    }
    
    { PROFILE("Update physics");
        Managers().physicsManager->Update(world, deltaTime);
    }

    { PROFILE("Update animations");
        for (Entity* entity : world.GetEntities()) {
            Component::Animation* anim = entity->GetComponent<Component::Animation>();
            if (anim != nullptr) {
                Geometry::RiggedModel* model = anim->riggedModel;
                if (model != nullptr) {
                    if (!model->animations.empty()) {
                        anim->time += deltaTime;
                        model->skeleton.Animate(&model->animations[0], anim->time);
                    }
                }
            }
        }
    }
    
    { PROFILE("Update particles");
        Managers().particleManager->Update(world, deltaTime);
    }
    
    { PROFILE("Update sounds");
        Managers().soundManager->Update(world);
    }
    
    { PROFILE("Update debug drawing");
        Managers().debugDrawingManager->Update(deltaTime);
    }
    
    { PROFILE("Clear killed entities/components");
        world.ClearKilled();
    }
}

void ActiveHymn::Render(bool soundSources, bool particleEmitters, bool lightSources) {
    { PROFILE("Render world");
        Managers().renderManager->Render(world);
    }
    
    if (soundSources || particleEmitters || lightSources) {
        { PROFILE("Render editor entities");
            Managers().renderManager->RenderEditorEntities(world, soundSources, particleEmitters, lightSources);
        }
    }
    
    { PROFILE("Render debug entities");
        Managers().debugDrawingManager->Render(world);
    }
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
