#include "Entity.hpp"

#include "../Component/Transform.hpp"
#include "../Component/Lens.hpp"

Entity::Entity(Scene* scene, const std::string& name) {
    this->scene = scene;
    this->name = name;
}

Entity::~Entity() {
    
}

Json::Value Entity::Save() const {
    Json::Value entity;
    entity["name"] = name;
    
    Save<Component::Transform>(entity, "Transform");
    Save<Component::Lens>(entity, "Lens");
    
    return entity;
}

void Entity::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    
    Load<Component::Transform>(node, "Transform");
    Load<Component::Lens>(node, "Lens");
}
