#include "Entity.hpp"

Entity::Entity(Scene* scene, const std::string& name) {
    this->scene = scene;
    this->name = name;
}

Entity::~Entity() {
    
}

Json::Value Entity::Save() const {
    Json::Value entity;
    entity["name"] = name;
    return entity;
}

void Entity::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
}
