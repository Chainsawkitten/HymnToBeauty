#include "Entity.hpp"

#include "../Component/Transform.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"

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
    Save<Component::Mesh>(entity, "Mesh");
    Save<Component::DirectionalLight>(entity, "DirectionalLight");
    Save<Component::PointLight>(entity, "PointLight");
    Save<Component::SpotLight>(entity, "SpotLight");
    
    return entity;
}

void Entity::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    
    Load<Component::Transform>(node, "Transform");
    Load<Component::Lens>(node, "Lens");
    Load<Component::Mesh>(node, "Mesh");
    Load<Component::DirectionalLight>(node, "DirectionalLight");
    Load<Component::PointLight>(node, "PointLight");
    Load<Component::SpotLight>(node, "SpotLight");
}
