#include "SuperComponent.hpp"

#include "../Entity/Entity.hpp"

using namespace Component;

SuperComponent::SuperComponent(Entity* entity) {
    this->entity = entity;
}

SuperComponent::~SuperComponent() {
    
}

Json::Value SuperComponent::Save() const {
    return Json::Value();
}

void SuperComponent::Load(const Json::Value& node) {
    
}
