#pragma once

#include <vector>
#include <map>
#include <typeinfo>
#include "../Manager/ParticleManager.hpp"

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
         * @param name Name of the entity to create.
         * @return The new entity.
         */
        Entity* CreateEntity(const std::string& name = "");
        
        /// Get all the entities in the scene.
        /**
         * @return The entities in the scene.
         */
        const std::vector<Entity*>& GetEntities() const;
        
        /// Gets all components of a specific type.
        /**
         * @return A list of pointers to all components of the specified scene.
         */
        template<typename T> std::vector<T*>& GetComponents();
        
        /// Register an entity to receive update events.
        /**
         * @param entity %Entity to register.
         */
        void RegisterUpdate(Entity* entity);
        
        /// Get all entities that are registered to receive update events.
        /**
         * @return A list of all entities registered to receive update events.
         */
        const std::vector<Entity*>& GetUpdateEntities() const;
        
        /// Clear the scene of all entities.
        void Clear();
        
        /// Removes all killed entities and components in the scene.
        void ClearKilled();
        
        /// Get all the particles in the scene.
        /**
         * @return Array of all the particles in the scene.
         */
        ParticleManager::Particle* GetParticles() const;
        
        /// Get the number of particles in the scene.
        /**
         * @return The number of particles in the scene.
         */
        unsigned int GetParticleCount() const;
        
        /// Set the number of particles in the scene.
        /**
         * @param particleCount The number of particles in the scene.
         */
        void SetParticleCount(unsigned int particleCount);
        
    private:
        // Add component.
        void AddComponent(Component::SuperComponent* component, const std::type_info* componentType);
        
        // List of all entities in this scene.
        std::vector<Entity*> entities;
        
        // Map containing list of components.
        std::map<const std::type_info*, std::vector<Component::SuperComponent*>> components;
        
        // All particles in the scene.
        ParticleManager::Particle* particles;
        unsigned int particleCount = 0;
        
        // Entities registered for update event.
        std::vector<Entity*> updateEntities;
};

template<typename T> inline std::vector<T*>& Scene::GetComponents() {
    return reinterpret_cast<std::vector<T*>&>(components[&typeid(T*)]);
}
