#pragma once

#include <vector>
#include <map>
#include <typeinfo>

class Entity;
namespace Component {
    class SuperComponent;
}

/// A scene containing entities.
class Scene {
    friend class Entity;
    
    public:
        /// Create a new scene.
        Scene();
        
        /// Destructor.
        ~Scene();
        
        /// Create a new entity in the scene.
        /**
         * @return The new entity.
         */
        Entity* CreateEntity();
        
        /// Gets all components of a specific type.
        /**
         * @return A list of pointers to all components of the specified scene.
         */
        template<typename T> std::vector<T*>& GetComponents();
        
        /// Clear the scene of all entities.
        void Clear();
        
    private:
        // Add component.
        void AddComponent(Component::SuperComponent* component, const std::type_info* componentType);
        
        // List of all entities in this scene.
        std::vector<Entity*> entities;
        
        // Map containing list of components.
        std::map<const std::type_info*, std::vector<Component::SuperComponent*>> components;
};

template<typename T> inline std::vector<T*>& Scene::GetComponents() {
    return reinterpret_cast<std::vector<T*>&>(components[&typeid(T*)]);
}
