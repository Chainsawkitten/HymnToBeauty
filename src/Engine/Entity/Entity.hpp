#pragma once

#include <map>
#include <typeinfo>
#include "../Scene/Scene.hpp"

namespace Component {
    class SuperComponent;
}

/// %Entity containing various components.
class Entity {
    public:
        /// Create new entity.
        /**
         * @param scene The scene in which the entity is contained.
         */
        Entity(Scene* scene);
        
        /// Destructor.
        ~Entity();
        
        /// Adds component with type T.
        /**
         * @return The created component.
         */
        template<typename T> T* AddComponent();
        
    private:
        Scene* scene;
        
        std::map<const std::type_info*, Component::SuperComponent*> components;
};

template <typename T> T* Entity::AddComponent() {
    const std::type_info* componentType = &typeid(T*);
    if (components.find(componentType) != components.end())
        return nullptr;
    T* component = new T(this);
    components[componentType] = component;
    scene->AddComponent(component, componentType);
    return component;
}
