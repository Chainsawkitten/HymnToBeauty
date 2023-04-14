#include "Entity.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../Component/Camera.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/RigidBody.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Script.hpp"
#include "../Component/Shape.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/Sprite.hpp"
#include "../Component/Trigger.hpp"
#include "../Util/Json.hpp"
#include "../Util/FileSystem.hpp"
#include <Utility/Log.hpp>
#include "../Hymn.hpp"
#include <fstream>
#include "../Manager/Managers.hpp"
#include "../Manager/PhysicsManager.hpp"
#include "../Manager/RenderManager.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Manager/SoundManager.hpp"
#include "../Manager/TriggerManager.hpp"

Entity::Entity(World* world, const std::string& name) : name(name) {
    this->world = world;
}

Entity::~Entity() {}

Entity* Entity::GetParent() const {
    return parent;
}

Entity* Entity::AddChild(const std::string& name) {
    Entity* child = world->CreateEntity(name);
    child->parent = this;
    children.push_back(child);
    return child;
}

Entity* Entity::SetParent(Entity* newParent) {
    // We make sure we're not trying to put the root as a child.
    if (parent != nullptr) {
        // We make sure we're not trying to set a parent as a child to one of it's own children.
        if (!HasChild(newParent)) {
            parent->RemoveChild(this);
            Entity* lastParent = parent;
            parent = newParent;
            newParent->children.push_back(this);

            // The entity we're relative to has changed, so recalculate world matrix.
            SetDirty(DirtyFlag::WORLD_MATRIX);

            return lastParent;
        }
    }
    return nullptr;
}

bool Entity::HasChild(const Entity* check_child, bool deep) const {
    for (Entity* child : children) {
        if (child->name == check_child->name)
            return true;
        else if (deep)
            child->HasChild(check_child);
    }

    return false;
}

Entity* Entity::InstantiateScene(const std::string& name) {
    return InstantiateScene(name, "");
}

Entity* Entity::InstantiateScene(const std::string& name, const std::string& originScene) {
    Json::Value root;
    Entity* child = AddChild();
    bool error = false;
    std::string filename = Hymn().GetPath() + "/" + name;

    // Checks if file exists.
    if (FileSystem::FileExists(filename.c_str())) {
        std::ifstream file1(filename);
        file1 >> root;

        if (!originScene.empty())
            CheckIfSceneExists(filename, error, originScene, root);

        if (error == false) {
            std::ifstream file(filename);
            file >> root;
            file.close();
            child->Serialize(root, true);
            child->scene = true;
            child->sceneName = name;
            Managers().triggerManager->InitiateUID();
            Managers().triggerManager->InitiateVolumes();
        }
    } else {
        child->name = "Error loading scene";
        Log() << "Couldn't find scene to load: " << filename;
    }

    if (error) {
        child->name = "Error loading scene";
        Log() << "Scene is added in continous loop: " << filename;
    }

    return child;
}

void Entity::CheckIfSceneExists(const std::string& filename, bool& error, const std::string& originScene, Json::Value root) {
    // Loops through all the scene names.
    for (unsigned int i = 0; i < root["children"].size(); ++i) {
        if (root["children"][i]["scene"].asBool()) {
            printf("%s", root["children"][i]["sceneName"].asString().c_str());

            if (originScene == root["children"][i]["sceneName"].asString())
                error = true;

            if (error)
                break;
        }

        if (!error)
            CheckIfSceneExists(filename, error, originScene, root["children"][i]);
    }
}

const std::vector<Entity*>& Entity::GetChildren() const {
    return children;
}

Entity* Entity::GetChild(const std::string& name) const {
    for (Entity* child : children) {
        if (child->name == name)
            return child;
    }

    return nullptr;
}

Entity* Entity::GetChildFromIndex(int index) const {
    if (children.size() > index)
        return children[index];

    return nullptr;
}

unsigned int Entity::GetNumChildren() const {
    return (unsigned int)children.size();
}

bool Entity::RemoveChild(Entity* child) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == child) {
            children.erase(it);
            return true;
        }
    }

    return false;
}

bool Entity::IsScene() const {
    return scene;
}

void Entity::Kill() {
    KillHelper();

    // Remove this entity from the parent's list of children.
    if (parent != nullptr)
        parent->RemoveChild(this);
}

bool Entity::IsKilled() const {
    return killed;
}

void Entity::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "scene", scene, false);

    if (scene) {
        Json::Serialize(node, load, "sceneName", sceneName, "");

        if (load) {
            // Load scene.
            std::string filename = Hymn().GetPath() + "/" + sceneName;
            Json::Value root;
            std::ifstream file(filename);
            file >> root;
            file.close();
            Serialize(root, true);

            scene = true;
        }
    } else {
        Serialize<Component::Camera>(node, load, "Camera");
        Serialize<Component::Mesh>(node, load, "Mesh");
        Serialize<Component::Material>(node, load, "Material");
        Serialize<Component::DirectionalLight>(node, load, "DirectionalLight");
        Serialize<Component::PointLight>(node, load, "PointLight");
        Serialize<Component::SpotLight>(node, load, "SpotLight");
        Serialize<Component::RigidBody>(node, load, "RigidBody");
        Serialize<Component::Listener>(node, load, "Listener");
        Serialize<Component::Script>(node, load, "Script");
        Serialize<Component::Shape>(node, load, "Shape");
        Serialize<Component::SoundSource>(node, load, "SoundSource");
        Serialize<Component::Sprite>(node, load, "Sprite");
        Serialize<Component::Trigger>(node, load, "Trigger");

        if (load) {
            // Load children.
            for (unsigned int i = 0; i < node["children"].size(); ++i) {
                Entity* entity = AddChild("");
                entity->Serialize(node["children"][i], true);
            }
        } else {
            // Save children.
            Json::Value childNodes;
            for (Entity* child : children) {
                Json::Value childNode;
                child->Serialize(childNode, false);
                childNodes.append(childNode);
            }
            node["children"] = childNodes;
        }
    }

    Json::Serialize(node, load, "name", name, "");
    Json::Serialize(node, load, "position", position, glm::vec3(0.0f, 0.0f, 0.0f));
    Json::Serialize(node, load, "scale", scale, glm::vec3(1.0f, 1.0f, 1.0f));
    Json::Serialize(node, load, "rotation", rotation, glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
    Json::Serialize(node, load, "uid", uniqueIdentifier, 0u);

    if (load) {
        dirtyMask = DirtyFlag::LOCAL_MATRIX | DirtyFlag::WORLD_MATRIX;
    }
}

const glm::vec3& Entity::GetPosition() const {
    return position;
}

void Entity::SetPosition(const glm::vec3& position) {
    this->position = position;
    SetDirty();
}

glm::vec3 Entity::GetWorldPosition() const {
    if (parent != nullptr) {
        const glm::mat4 matrix = GetWorldModelMatrix();
        return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
    }

    return position;
}

void Entity::SetWorldPosition(const glm::vec3& worldPos) {
    if (parent == nullptr) {
        SetPosition(worldPos);
    } else {
        SetPosition(glm::inverse(parent->GetWorldModelMatrix()) * glm::vec4(worldPos, 1.0f));
    }
}

void Entity::Move(const glm::vec3& translation) {
    SetPosition(GetPosition() + translation);
}

const glm::vec3& Entity::GetScale() const {
    return scale;
}

void Entity::SetScale(const glm::vec3& scale) {
    this->scale = scale;
    SetDirty();
}

const glm::quat& Entity::GetRotation() const {
    return rotation;
}

void Entity::SetRotation(const glm::quat& localRot) {
    rotation = localRot;
    SetDirty();
}

glm::quat Entity::GetWorldRotation() const {
    if (parent != nullptr) {
        auto transform = parent->GetWorldModelMatrix();
        transform = transform * glm::mat4_cast(rotation);
        return glm::quat_cast(transform);
    }

    return rotation;
}

void Entity::SetWorldRotation(const glm::quat& worldRot) {
    if (parent == nullptr) {
        SetRotation(worldRot);
    } else {
        glm::quat quater = glm::quat_cast(parent->GetWorldModelMatrix());
        quater = glm::inverse(quater) * worldRot;
        SetRotation(quater);
    }
}

glm::vec3 Entity::GetWorldDirection() const {
    return glm::vec3(GetWorldRotation() * glm::vec3(0, 0, -1));
}

void Entity::RotateYaw(float angle) {
    SetRotation(glm::rotate(GetRotation(), angle, glm::vec3(0, 1, 0)));
}

void Entity::RotatePitch(float angle) {
    SetRotation(glm::rotate(GetRotation(), angle, glm::vec3(1, 0, 0)));
}

void Entity::RotateRoll(float angle) {
    SetRotation(glm::rotate(GetRotation(), angle, glm::vec3(0, 0, 1)));
}

void Entity::RotateAroundWorldAxis(float angle, const glm::vec3& axis) {
    glm::quat invQuat = glm::inverse(glm::quat_cast(GetWorldModelMatrix()));
    glm::vec3 tempVec = glm::rotate(invQuat, axis);
    SetRotation(glm::rotate(GetRotation(), angle, tempVec));
}

const glm::mat4& Entity::GetLocalMatrix() const {
    if (dirtyMask & DirtyFlag::LOCAL_MATRIX) {
        // Calculate rotation * scale.
        glm::mat3 rotationScaleMatrix = glm::mat3_cast(GetRotation());
        rotationScaleMatrix[0] *= scale.x;
        rotationScaleMatrix[1] *= scale.y;
        rotationScaleMatrix[2] *= scale.z;

        // Position.
        localMatrix = glm::mat4(rotationScaleMatrix);
        localMatrix[3] = glm::vec4(GetPosition(), 1.0f);

        dirtyMask &= ~DirtyFlag::LOCAL_MATRIX;
    }

    return localMatrix;
}

const glm::mat4& Entity::GetWorldModelMatrix() const {
    if (dirtyMask & DirtyFlag::WORLD_MATRIX) {
        worldMatrix = GetLocalMatrix();

        if (parent != nullptr)
            worldMatrix = parent->GetWorldModelMatrix() * worldMatrix;

        dirtyMask &= ~DirtyFlag::WORLD_MATRIX;
    }

    return worldMatrix;
}

void Entity::SetEnabled(bool enabled, bool recursive) {
    this->enabled = enabled;

    if (recursive) {
        for (Entity* child : children)
            child->SetEnabled(enabled, true);
    }
}

bool Entity::IsEnabled() const {
    return enabled;
}

unsigned int Entity::GetUniqueIdentifier() const {
    return uniqueIdentifier;
}

void Entity::SetUniqueIdentifier(unsigned int UID) {
    uniqueIdentifier = UID;
}

Component::SuperComponent* Entity::AddComponent(std::type_index componentType) {
    // Check if component already exists.
    if (components.find(componentType) != components.end())
        return nullptr;

    Component::SuperComponent* component;

    // Create a component in the correct manager.
    if (componentType == typeid(Component::DirectionalLight*))
        component = Managers().renderManager->CreateDirectionalLight();
    else if (componentType == typeid(Component::Camera*))
        component = Managers().renderManager->CreateCamera();
    else if (componentType == typeid(Component::Listener*))
        component = Managers().soundManager->CreateListener();
    else if (componentType == typeid(Component::Material*))
        component = Managers().renderManager->CreateMaterial();
    else if (componentType == typeid(Component::Mesh*))
        component = Managers().renderManager->CreateMesh();
    else if (componentType == typeid(Component::PointLight*))
        component = Managers().renderManager->CreatePointLight();
    else if (componentType == typeid(Component::RigidBody*))
        component = Managers().physicsManager->CreateRigidBody(this);
    else if (componentType == typeid(Component::Script*))
        component = Managers().scriptManager->CreateScript();
    else if (componentType == typeid(Component::Shape*))
        component = Managers().physicsManager->CreateShape(this);
    else if (componentType == typeid(Component::SoundSource*))
        component = Managers().soundManager->CreateSoundSource();
    else if (componentType == typeid(Component::SpotLight*))
        component = Managers().renderManager->CreateSpotLight();
    else if (componentType == typeid(Component::Sprite*))
        component = Managers().renderManager->CreateSprite();
    else if (componentType == typeid(Component::Trigger*))
        component = Managers().triggerManager->CreateTrigger();
    else {
        Log() << componentType.name() << " not assigned to a manager!"
              << "\n";
        return nullptr;
    }

    // Add component to our map.
    components[componentType] = component;

    // Set ourselves as the owner.
    component->entity = this;

    return component;
}

Component::SuperComponent* Entity::GetComponent(std::type_index componentType) const {
    auto it = components.find(componentType);
    if (it != components.end())
        return it->second;
    else
        return nullptr;
}

void Entity::KillComponent(std::type_index componentType) {
    if (components.find(componentType) != components.end()) {
        components[componentType]->Kill();
        components.erase(componentType);
    }
}

void Entity::KillHelper() {
    killed = true;

    for (auto& it : components)
        it.second->Kill();

    for (Entity* child : children) {
        child->KillHelper();
    }
}

void Entity::SetDirty(uint32_t dirtyMask) {
    if ((this->dirtyMask & dirtyMask) == dirtyMask) {
        // We (and by extension all children) are already dirty. No need to do anything.
        return;
    }

    this->dirtyMask |= dirtyMask;

    for (Entity* child : children) {
        child->SetDirty(DirtyFlag::WORLD_MATRIX);
    }
}
