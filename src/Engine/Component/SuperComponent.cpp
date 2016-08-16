#include "SuperComponent.hpp"

#include "../Entity/Entity.hpp"

using namespace Component;

SuperComponent::SuperComponent(Entity* entity) {
    this->entity = entity;
}

SuperComponent::~SuperComponent() {
    
}

void SuperComponent::Kill() {
    killed = true;
}

bool SuperComponent::IsKilled() const {
    return killed;
}

Json::Value SuperComponent::Save() const {
    return Json::Value();
}

void SuperComponent::Load(const Json::Value& node) {
    
}
