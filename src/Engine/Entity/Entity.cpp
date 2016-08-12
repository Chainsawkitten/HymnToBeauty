#include "Entity.hpp"

#include "../Component/Transform.hpp"

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
    
    return entity;
}

void Entity::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    
    Json::Value componentNode = node["Transform"];
    if (!componentNode.isNull()) {
        Component::Transform* component = AddComponent<Component::Transform>();
        component->Load(componentNode);
    }
}
