#pragma once

#include <vector>
#include <map>

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
