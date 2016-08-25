#include "Entity.hpp"

#include "../Component/Transform.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/Physics.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"

Entity::Entity(Scene* scene, const std::string& name) {
    this->scene = scene;
    this->name = name;
}

Entity::~Entity() {
    
}

void Entity::Kill() {
    killed = true;
    
    for (auto& it : components)
        it.second->Kill();
}

bool Entity::IsKilled() const {
    return killed;
}

Json::Value Entity::Save() const {
    Json::Value entity;
    entity["name"] = name;
    
    Save<Component::Transform>(entity, "Transform");
    Save<Component::Lens>(entity, "Lens");
    Save<Component::Mesh>(entity, "Mesh");
    Save<Component::Material>(entity, "Material");
    Save<Component::DirectionalLight>(entity, "DirectionalLight");
    Save<Component::PointLight>(entity, "PointLight");
    Save<Component::SpotLight>(entity, "SpotLight");
    Save<Component::Physics>(entity, "Physics");
    Save<Component::Listener>(entity, "Listener");
    Save<Component::SoundSource>(entity, "SoundSource");
    
    return entity;
}

void Entity::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    
    Load<Component::Transform>(node, "Transform");
    Load<Component::Lens>(node, "Lens");
    Load<Component::Mesh>(node, "Mesh");
    Load<Component::Material>(node, "Material");
    Load<Component::DirectionalLight>(node, "DirectionalLight");
    Load<Component::PointLight>(node, "PointLight");
    Load<Component::SpotLight>(node, "SpotLight");
    Load<Component::Physics>(node, "Physics");
    Load<Component::Listener>(node, "Listener");
    Load<Component::SoundSource>(node, "SoundSource");
}
