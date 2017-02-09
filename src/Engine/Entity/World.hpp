#pragma once

#include <vector>
#include <map>
#include <typeinfo>
#include "../Manager/ParticleManager.hpp"

class Entity;
namespace Component {
    class SuperComponent;
}

/// The game world containing all entities.
class World {
    friend class Entity;
    
    public:
        /// Create a new world.
        World();
        
        /// Destructor.
        ~World();
        
        /// Create a new entity in the world.
        /**
         * @param name Name of the entity to create.
         * @return The new entity.
         */
        Entity* CreateEntity(const std::string& name = "");
        
        /// Get all the entities in the world.
        /**
         * @return The entities in the world.
         */
        const std::vector<Entity*>& GetEntities() const;
        
        /// Gets all components of a specific type.
        /**
         * @return A list of pointers to all components of the world.
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
        
        /// Clear the world of all entities.
        void Clear();
        
        /// Removes all killed entities and components in the world.
        void ClearKilled();
        
        /// Get all the particles in the world.
        /**
         * @return Array of all the particles in the world.
         */
        ParticleManager::Particle* GetParticles() const;
        
        /// Get the number of particles in the world.
        /**
         * @return The number of particles in the world.
         */
        unsigned int GetParticleCount() const;
        
        /// Set the number of particles in the world.
        /**
         * @param particleCount The number of particles in the world.
         */
        void SetParticleCount(unsigned int particleCount);
        
        /// Save the world to file.
        /**
         * @param filename The name of the file.
         */
        void Save(const std::string& filename) const;
        
        /// Load the world from file.
        /**
         * @param filename The name of the file.
         */
        void Load(const std::string& filename);
        
    private:
        // Add component.
        void AddComponent(Component::SuperComponent* component, const std::type_info* componentType);
        
        // List of all entities in this world.
        std::vector<Entity*> entities;
        
        // Map containing list of components.
        std::map<const std::type_info*, std::vector<Component::SuperComponent*>> components;
        
        // All particles in the world.
        ParticleManager::Particle* particles;
        unsigned int particleCount = 0;
        
        // Entities registered for update event.
        std::vector<Entity*> updateEntities;
};

template<typename T> inline std::vector<T*>& World::GetComponents() {
    return reinterpret_cast<std::vector<T*>&>(components[&typeid(T*)]);
}
