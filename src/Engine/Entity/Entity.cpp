#include "Entity.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../Component/Animation.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/Physics.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Script.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Util/Json.hpp"
#include "../Util/FileSystem.hpp"
#include "../Util/Log.hpp"
#include "../Hymn.hpp"
#include <fstream>

Entity::Entity(World* world, const std::string& name) {
    this->world = world;
    this->name = name;
}

Entity::~Entity() {
    
}

Entity* Entity::AddChild(const std::string& name) {
    Entity* child = world->CreateEntity(name);
    child->parent = this;
    children.push_back(child);
    return child;
}

Entity* Entity::InstantiateScene(const std::string& name) {
    Entity* child = AddChild();
    
    // Load scene.
    std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + name + ".json";
    if (FileSystem::FileExists(filename.c_str())) {
        Json::Value root;
        std::ifstream file(filename);
        file >> root;
        file.close();
        child->Load(root);

        child->scene = true;
        child->sceneName = name;
    }
    else {

        child->name = "Error loading scene";
        Log() << "Couldn't find scene to load.";

    }

    return child;
}

const std::vector<Entity*>& Entity::GetChildren() const {
    return children;
}

bool Entity::IsScene() const {
    return scene;
}

void Entity::Kill() {
    killed = true;
    
    for (auto& it : components)
        it.second->Kill();
    
    for (Entity* child : children)
        child->Kill();
    
    // Remove this entity from the parent's list of children.
    if (parent != nullptr && !parent->killed) {
        for (auto it = parent->children.begin(); it != parent->children.end(); ++it) {
            if (*it == this) {
                parent->children.erase(it);
                break;
            }
        }
    }
}

bool Entity::IsKilled() const {
    return killed;
}

Json::Value Entity::Save() const {
    Json::Value entity;
    entity["name"] = name;
    entity["position"] = Json::SaveVec3(position);
    entity["scale"] = Json::SaveVec3(scale);
    entity["rotation"] = Json::SaveVec3(rotation);
    entity["scene"] = scene;
    
    if (scene) {
        entity["sceneName"] = sceneName;
    } else {
        // Save components.
        Save<Component::Animation>(entity, "Animation");
        Save<Component::Lens>(entity, "Lens");
        Save<Component::Mesh>(entity, "Mesh");
        Save<Component::Material>(entity, "Material");
        Save<Component::DirectionalLight>(entity, "DirectionalLight");
        Save<Component::PointLight>(entity, "PointLight");
        Save<Component::SpotLight>(entity, "SpotLight");
        Save<Component::Physics>(entity, "Physics");
        Save<Component::Listener>(entity, "Listener");
        Save<Component::Script>(entity, "Script");
        Save<Component::SoundSource>(entity, "SoundSource");
        Save<Component::ParticleEmitter>(entity, "ParticleEmitter");
        
        // Save children.
        Json::Value childNodes;
        for (Entity* child : children)
            childNodes.append(child->Save());
        entity["children"] = childNodes;
    }
    
    return entity;
}

void Entity::Load(const Json::Value& node) {
    scene = node["scene"].asBool();
    
    if (scene) {
        sceneName = node["sceneName"].asString();
        
        // Load scene.
        std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + sceneName + ".json";
        Json::Value root;
        std::ifstream file(filename);
        file >> root;
        file.close();
        Load(root);
        
        scene = true;
    } else {
        // Load components.
        Load<Component::Animation>(node, "Animation");
        Load<Component::Lens>(node, "Lens");
        Load<Component::Mesh>(node, "Mesh");
        Load<Component::Material>(node, "Material");
        Load<Component::DirectionalLight>(node, "DirectionalLight");
        Load<Component::PointLight>(node, "PointLight");
        Load<Component::SpotLight>(node, "SpotLight");
        Load<Component::Physics>(node, "Physics");
        Load<Component::Listener>(node, "Listener");
        Load<Component::Script>(node, "Script");
        Load<Component::SoundSource>(node, "SoundSource");
        Load<Component::ParticleEmitter>(node, "ParticleEmitter");
        
        // Load children.
        for (unsigned int i=0; i < node["children"].size(); ++i) {
            Entity* entity = AddChild("");
            entity->Load(node["children"][i]);
        }
    }
    
    name = node.get("name", "").asString();
    position = Json::LoadVec3(node["position"]);
    scale = Json::LoadVec3(node["scale"]);
    rotation = Json::LoadVec3(node["rotation"]);
}

glm::mat4 Entity::GetModelMatrix() const {
    glm::mat4 matrix = glm::translate(glm::mat4(), position) * GetOrientation() * glm::scale(glm::mat4(), scale);
    
    if (parent != nullptr)
        matrix = parent->GetModelMatrix() * matrix;
    
    return matrix;
}

glm::mat4 Entity::GetOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
}

glm::mat4 Entity::GetCameraOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
}

glm::vec3 Entity::GetDirection() const {
    return glm::normalize(glm::vec3(GetOrientation() * glm::vec4(0.f, 0.f, 1.f, 0.f)));
}

glm::vec3 Entity::GetWorldPosition() const {
    if (parent != nullptr)
        return glm::vec3(parent->GetModelMatrix() * glm::vec4(position, 1.f));
    
    return position;
}
