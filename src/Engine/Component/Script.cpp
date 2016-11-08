#include "Script.hpp"

using namespace Component;

Script::Script(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Script::Save() const {
    Json::Value component;
    component["placeholderValue"] = "";
    return component;
}

void Script::Load(const Json::Value& node) {
    
}
