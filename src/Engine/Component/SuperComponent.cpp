#include "SuperComponent.hpp"

#include "../Entity/Entity.hpp"

using namespace Component;

SuperComponent::SuperComponent(Entity* entity) {
    this->entity = entity;
}

SuperComponent::~SuperComponent() {
    
}
