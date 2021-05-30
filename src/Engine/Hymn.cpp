#include "Hymn.hpp"

#include "Util/FileSystem.hpp"
#include "Manager/Managers.hpp"
#include "Manager/PhysicsManager.hpp"
#include "Manager/ScriptManager.hpp"
#include "Manager/SoundManager.hpp"
#include "Manager/TriggerManager.hpp"
#include "Manager/DebugDrawingManager.hpp"
#include "Manager/ResourceManager.hpp"
#include "Geometry/Model.hpp"
#include "Input/Input.hpp"
#include "Script/ScriptFile.hpp"
#include "Util/Json.hpp"
#include <fstream>
#include "Util/Profiling.hpp"
#include "Entity/Entity.hpp"

// Fix windows.h pollution.
#ifdef _WIN32
#undef CreateDirectory
#endif

using namespace std;

ActiveHymn::ActiveHymn() {
    Clear();
}

ActiveHymn& ActiveHymn::GetInstance() {
    static ActiveHymn ActiveHymn;

    return ActiveHymn;
}

void ActiveHymn::Clear() {
    path = "";
    startupScene = "";
    name = "";
    world.Clear();

    entityNumber = 1U;

    filterSettings.gamma = 2.2f;
    filterSettings.fxaa = true;

    for (ScriptFile* script : scripts) {
        Managers().resourceManager->FreeScriptFile(script);
    }
    scripts.clear();
    scriptNumber = 0U;
}

const string& ActiveHymn::GetPath() const {
    return path;
}

void ActiveHymn::SetPath(const string& path) {
    this->path = path;
    FileSystem::CreateDirectory(path.c_str());
    FileSystem::CreateDirectory((path + "/Resources").c_str());
}

std::string ActiveHymn::GetSavePath() const {
    return path + FileSystem::DELIMITER + "Hymn.json";
}

void ActiveHymn::Save() const {
    // Save to file.
    ofstream file(GetSavePath());
    file << ToJson();
    file.close();
}

void ActiveHymn::Load(const string& path) {
    Clear();
    this->path = path;

    // Load Json document from file.
    Json::Value root;
    ifstream file(GetSavePath());
    file >> root;
    file.close();

    FromJson(root);
}

Json::Value ActiveHymn::ToJson() const {
    Json::Value root;

    Json::Value inputNode;
    inputNode.append(Input::GetInstance().Save());
    root["input"] = inputNode;

    // Filter settings.
    Json::Value filtersNode;
    filtersNode["gamma"] = filterSettings.gamma;
    filtersNode["dither"] = filterSettings.ditherApply;
    filtersNode["fxaa"] = filterSettings.fxaa;
    filtersNode["bloom"] = filterSettings.bloom;
    filtersNode["bloomIntensity"] = filterSettings.bloomIntensity;
    filtersNode["bloomThreshold"] = filterSettings.bloomThreshold;
    filtersNode["bloomScatter"] = filterSettings.bloomScatter;
    root["filters"] = filtersNode;

    // Save scripts.
    Json::Value scriptNode;
    for (ScriptFile* script : scripts) {
        scriptNode.append(script->path + script->name);
    }
    root["scripts"] = scriptNode;

    root["startupScene"] = startupScene;
    root["name"] = name;

    return root;
}

void ActiveHymn::FromJson(Json::Value root) {
    const Json::Value inputNode = root["input"];
    Input::GetInstance().Load(inputNode[0]);

    // Load filter settings.
    Json::Value filtersNode = root["filters"];
    filterSettings.gamma = filtersNode.get("gamma", 2.2f).asFloat();
    filterSettings.ditherApply = filtersNode["dither"].asBool();
    filterSettings.fxaa = filtersNode["fxaa"].asBool();
    filterSettings.bloom = filtersNode["bloom"].asBool();
    filterSettings.bloomIntensity = filtersNode.get("bloomIntensity", 1.0f).asFloat();
    filterSettings.bloomThreshold = filtersNode.get("bloomThreshold", 1.0f).asFloat();
    filterSettings.bloomScatter = filtersNode.get("bloomScatter", 0.7f).asFloat();

    // Load scripts.
    scripts.clear();
    const Json::Value scriptNode = root["scripts"];
    for (unsigned int i = 0; i < scriptNode.size(); ++i) {
        scripts.push_back(Managers().resourceManager->CreateScriptFile(scriptNode[i].asString()));
    }
    scriptNumber = static_cast<unsigned int>(scripts.size());

    startupScene = root["startupScene"].asString();
    name = root["name"].asString();
}

void ActiveHymn::Update(float deltaTime) {
    {
        PROFILE("Run scripts.");
        Managers().scriptManager->Update(world, deltaTime);
    }

    {
        PROFILE("Synchronize triggers.");
        Managers().triggerManager->SynchronizeTriggers();
    }

    {
        PROFILE("Update physics");
        Managers().physicsManager->Update(deltaTime);
    }

    {
        PROFILE("Update debug drawing");
        Managers().debugDrawingManager->Update(deltaTime);
    }

    {
        PROFILE("Synchronize transforms");
        Managers().physicsManager->UpdateEntityTransforms();
    }

    {
        PROFILE("Process triggers");
        Managers().triggerManager->ProcessTriggers();
    }

    {
        PROFILE("Clear killed entities/components");
        world.ClearKilled();
    }

    if (restart) {
        restart = false;
        FromJson(saveStateHymn);
        world.Load(saveStateWorld);
        Managers().scriptManager->RegisterInput();
        Managers().scriptManager->BuildAllScripts();
    }
}

void ActiveHymn::Render(Entity* camera, bool soundSources, bool lightSources, bool cameras, bool physics, bool lighting, bool lightVolumes) {
    PROFILE("Render world");

    Managers().renderManager->Render(world, soundSources, lightSources, cameras, physics, camera, lighting, lightVolumes);
}

Entity* ActiveHymn::GetEntityByGUID(unsigned int GUID) {
    const vector<Entity*>& entities = Hymn().world.GetEntities();
    for (size_t i = 0; i < entities.size(); ++i) {
        if (entities[i]->GetUniqueIdentifier() == GUID)
            return entities[i];
    }
    return nullptr;
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
