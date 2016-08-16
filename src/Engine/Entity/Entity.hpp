#pragma once

#include <map>
#include <typeinfo>
#include "../Scene/Scene.hpp"
#include <json/json.h>
#include "../Component/SuperComponent.hpp"

/// %Entity containing various components.
class Entity {
    public:
        /// Create new entity.
        /**
         * @param scene The scene in which the entity is contained.
         */
        Entity(Scene* scene, const std::string& name = "");
        
        /// Destructor.
        ~Entity();
        
        /// Adds component with type T.
        /**
         * @return The created component.
         */
        template<typename T> T* AddComponent();
        
        /// Gets component with type T.
        /**
         * @return The requested component (or nullptr).
         */
        template<typename T> T* GetComponent();
        
        /// Kill component of type T.
        template <typename T> void KillComponent();
        
        /// Kill the entity, will be removed at the end of the frame.
        void Kill();
        
        /// Get whether entity has been killed.
        /**
         * @return Whether the entity has been killed.
         */
        bool IsKilled() const;
        
        /// Save the entity.
        /**
         * @return JSON value to be stored on disk.
         */
        Json::Value Save() const;
        
        /// Load entity from JSON node.
        /**
         * @param node JSON node to load from.
         */
        void Load(const Json::Value& node);
        
        /// Name of the entity.
        std::string name;
        
    private:
        template<typename T> void Save(Json::Value& node, const std::string& name) const;
        template<typename T> void Load(const Json::Value& node, const std::string& name);
        
        Scene* scene;
        
        std::map<const std::type_info*, Component::SuperComponent*> components;
        
        bool killed = false;
};

template<typename T> T* Entity::AddComponent() {
    const std::type_info* componentType = &typeid(T*);
    if (components.find(componentType) != components.end())
        return nullptr;
    T* component = new T(this);
    components[componentType] = component;
    scene->AddComponent(component, componentType);
    return component;
}

template<typename T> T* Entity::GetComponent() {
    if (components.count(&typeid(T*)) != 0) {
        return static_cast<T*>(components[&typeid(T*)]);
    } else {
        return nullptr;
    }
}

template <typename T> void Entity::KillComponent() {
    const std::type_info* componentType = &typeid(T*);
    if (components.find(componentType) != components.end()) {
        components[componentType]->Kill();
        components.erase(componentType);
    }
}

template<typename T> void Entity::Save(Json::Value& node, const std::string& name) const {
    auto it = components.find(&typeid(T*));
    if (it != components.end())
        node[name] = it->second->Save();
}

template<typename T> void Entity::Load(const Json::Value& node, const std::string& name) {
    Json::Value componentNode = node[name];
    if (!componentNode.isNull()) {
        T* component = AddComponent<T>();
        component->Load(componentNode);
    }
}
