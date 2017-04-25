#include "Script.hpp"

#include <string>
#include <angelscript.h>
#include "../Manager/Managers.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Entity/Entity.hpp"
#include "../Script/ScriptFile.hpp"
#include "../Hymn.hpp"

using namespace Component;

Script::Script(Entity* entity) : SuperComponent(entity) {
    
}

Script::~Script() {
    if (instance != nullptr)
        instance->Release();
}

Json::Value Script::Save() const {
    Json::Value component;
    if (scriptFile != nullptr)
        component["scriptName"] = scriptFile->name;
    
    return component;
}

void Script::Load(const Json::Value& node) {
    std::string name = node.get("scriptName", "").asString();
    for (ScriptFile* currentScript : Hymn().scripts) {
        if (currentScript->name == name)
            this->scriptFile = currentScript;
    }
}
