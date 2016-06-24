#pragma once

#include <vector>

class Entity;

/// A scene containing entities.
class Scene {
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
        // List of all entities in this scene.
        std::vector<Entity*> entities;
};
