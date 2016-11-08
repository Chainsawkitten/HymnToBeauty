#include "Script.hpp"

#include <string>
#include "../Manager/Managers.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Entity/Entity.hpp"

using namespace Component;

Script::Script(Entity* entity) : SuperComponent(entity) {
    Managers().scriptManager->BuildScript(entity->name);
}

Json::Value Script::Save() const {
    Json::Value component;
    component["placeholderValue"] = "";
    return component;
}

void Script::Load(const Json::Value& node) {
    
}
