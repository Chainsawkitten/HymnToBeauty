#pragma once

#include <map>
#include <vector>
#include <typeindex>
#include <json/json.h>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "../Component/SuperComponent.hpp"
#include <fstream>

class World;

/// %Entity containing various components.
class Entity {
  public:
    /// Create new entity.
    /**
     * @param world The game world in which the entity is contained.
     * @param name Name of the entity.
     */
    Entity(World* world, const std::string& name);

    /// Destructor.
    ~Entity();

    /// Get the entity's parent entity.
    /**
     * @return The parent entity, or nullptr if none.
     */
    Entity* GetParent() const;

    /// Add child entity.
    /**
     * @param name The name of the child entity.
     * @return The new entity.
     */
    Entity* AddChild(const std::string& name = "");

    /// Remove child entity.
    /**
     * @param child The entity you want to remove.
     * @return Did we manage to remove the child.
     */
    bool RemoveChild(Entity* child);

    /// Set a new parent.
    /**
     * @param newParent The entity you want to be the new parent.
     * @return The old parent.
     */
    Entity* SetParent(Entity* newParent);

    /// Check if entity is a child.
    /**
     * @param child The entity you want to check if it is a child.
     * @param deep True if we want to check if it's a grandchild, false if we do not.
     * @return True if it has a child, false if it does not.
     */
    bool HasChild(const Entity* child, bool deep = true) const;

    /// Instantiate a scene as a child to this entity.
    /**
     * @param name The name of the scene to instantiate.
     * @return The created root entity of the scene.
     */
    Entity* InstantiateScene(const std::string& name);

    /// Instantiate a scene as a child to this entity.
    /**
     * @param name The name of the scene to instantiate.
     * @param originScene The name of the scene to instantiate within (to check for recursion).
     * @return The created root entity of the scene.
     */
    Entity* InstantiateScene(const std::string& name, const std::string& originScene);

    /// Check if scene already exists in any of json files.
    /**
     * @param filename The name of the scene to check.
     * @param error Set to true inside the function if it allready exists.
     * @param originScene Name of scene you want to check.
     * @param root The json value of root scene.
     */
    void CheckIfSceneExists(const std::string& filename, bool& error, const std::string& originScene, Json::Value root);

    /// Get all of the entity's children.
    /**
     * @return All the children.
     */
    const std::vector<Entity*>& GetChildren() const;

    /// Get child based on its name.
    /**
     * @param name The name of the child to get.
     * @return The child or nullptr if none was found.
     */
    Entity* GetChild(const std::string& name) const;

    /// Get child based on its name.
    /**
     * @param index The index of the child to get.
     * @return The child or nullptr if none was found.
     */
    Entity* GetChildFromIndex(int index) const;

    /// Get the number of children.
    /**
     * @return Number of children the entity has.
     */
    unsigned int GetNumChildren() const;

    /// Get whether the entity is an instantiated scene.
    /**
     * @return Whether the entity is an instantiated scene.
     */
    bool IsScene() const;

    /// Adds component with type T.
    /**
     * @return The created component.
     */
    template <typename T> T* AddComponent();

    /// Gets component with type T.
    /**
     * @return The requested component (or nullptr).
     */
    template <typename T> T* GetComponent() const;

    /// Kill component of type T.
    template <typename T> void KillComponent();

    /// Kill the entity, will be removed at the end of the frame.
    void Kill();

    /// Get whether entity has been killed.
    /**
     * @return Whether the entity has been killed.
     */
    bool IsKilled() const;

    /// Save or load entity to/from JSON.
    /**
     * @param node The JSON node to save to or load from.
     * @param load Whether to load (otherwise saves).
     */
    void Serialize(Json::Value& node, bool load);

    /// Get the model matrix.
    /**
     * @return The model matrix.
     */
    glm::mat4 GetModelMatrix() const;

    /// Get the local model matrix.
    /**
     * @return The local model matrix.
     */
    glm::mat4 GetLocalMatrix() const;

    /// Get the rotation of this entity.
    /**
     * @return The rotation in local space.
     */
    glm::quat GetLocalOrientation() const;

    /// Get orientation of the entity.
    /**
     * @return The rotation in world space.
     */
    glm::quat GetWorldOrientation() const;

    /// Get direction of the entity.
    /**
     * @return The entity's direction.
     */
    glm::vec3 GetDirection() const;

    /// Get the position in the world.
    /**
     * @return The position in the world (not relative to parent).
     */
    glm::vec3 GetWorldPosition() const;

    /// Set the position of the entity in world space.
    /**
     * @param worldPos The world position you want the entity to have.
     */
    void SetWorldPosition(const glm::vec3& worldPos);

    /// Set the orientation of the entity in world space.
    /**
     * @param worldRot New orientation.
     */
    void SetWorldOrientation(const glm::quat& worldRot);

    /// Set the local orientation of the entity.
    /**
     * @param localRot The local rotation you want the entity to have.
     */
    void SetLocalOrientation(const glm::quat& localRot);

    /// Rotates around the Y axis
    /**
     * @param angle The angle in radians that we want to rotate.
     */
    void RotateYaw(float angle);

    /// Rotates around the X axis
    /**
     * @param angle The angle in radians that we want to rotate.
     */
    void RotatePitch(float angle);

    /// Rotates around the Z axis
    /**
     * @param angle The angle in radians that we want to rotate.
     */
    void RotateRoll(float angle);

    /// Rotates around an axis given in world space.
    /**
     * @param angle The angle in radians that we want to rotate.
     * @param axis World space axis to rotate around.
     */
    void RotateAroundWorldAxis(float angle, const glm::vec3& axis);

    /// Set whether the entity should be enabled.
    /**
     * @param enabled Whether the entity should be enabled.
     * @param recursive Whether to set all children recursively.
     */
    void SetEnabled(bool enabled, bool recursive = false);

    /// Get whether the entity is enabled.
    /**
     * @return Whether the entity is enabled.
     */
    bool IsEnabled() const;

    /// Name of the entity.
    std::string name;

    /// Position relative to the parent entity.
    /**
     * Default: 0.f, 0.f, 0.f
     */
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    /// Scale.
    /**
     * Default: 1.f, 1.f, 1.f
     */
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    /// Quaternion describing rotation and angle of entity.
    /**
     * Default: 0 radians around y axis.
     */
    glm::quat rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    /// Get the entity's UID
    /**
     * @return The entity's UID
     */
    unsigned int GetUniqueIdentifier() const;

    /// Set the entity's UID
    /**
     * @param UID the entity's unique identifier to be set
     */
    void SetUniqueIdentifier(unsigned int UID);

    /// Variables used for enabling and disabling the paint brush tool.
    bool loadPaintModeClicked = false;
    bool brushActive = false;
    bool vertsLoaded = false;
    bool painting = false;
    bool sceneChosen = false;

  private:
    template <typename T> void Serialize(Json::Value& node, bool load, const std::string& name);
    Component::SuperComponent* AddComponent(std::type_index componentType);
    Component::SuperComponent* GetComponent(std::type_index componentType) const;
    void KillComponent(std::type_index componentType);
    void KillHelper();

    World* world;
    Entity* parent = nullptr;
    std::vector<Entity*> children;
    bool scene = false;
    std::string sceneName;

    std::map<std::type_index, Component::SuperComponent*> components;

    bool killed = false;
    bool enabled = true;
    unsigned int uniqueIdentifier = 0;
};

template <typename T> T* Entity::AddComponent() {
    std::type_index componentType = std::type_index(typeid(T*));
    return static_cast<T*>(AddComponent(componentType));
}

template <typename T> T* Entity::GetComponent() const {
    return static_cast<T*>(GetComponent(std::type_index(typeid(T*))));
}

template <typename T> void Entity::KillComponent() {
    KillComponent(typeid(T*));
}

template <typename T> void Entity::Serialize(Json::Value& node, bool load, const std::string& name) {
    std::type_index componentType = std::type_index(typeid(T*));
    Component::SuperComponent* component = nullptr;
    if (load) {
        if (!node[name].isNull()) {
            component = AddComponent(componentType);
        }
    } else {
        component = GetComponent(componentType);
    }

    if (component != nullptr) {
        component->Serialize(node[name], load);
    }
}
