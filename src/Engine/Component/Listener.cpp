#include "Listener.hpp"

using namespace Component;

Listener::Listener(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Listener::Save() const {
    Json::Value component;
    component["placeholderValue"] = "";
    return component;
}

void Listener::Load(const Json::Value& node) {
    
}
