#include "Listener.hpp"

using namespace Component;

Listener::Listener(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Listener::Save() const {
    return Json::Value();
}

void Listener::Load(const Json::Value& node) {
    
}
