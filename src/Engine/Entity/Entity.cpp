#include "Entity.hpp"

Entity::Entity(Scene* scene, const std::string& name) {
    this->scene = scene;
    this->name = name;
}

Entity::~Entity() {
    
}
