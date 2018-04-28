#pragma once

#include <vector>
#include <map>
#include <typeinfo>

class Entity;
namespace Json {
    class Value;
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

        /// Create root entity.
        void CreateRoot();

        /// Get the root entity.
        /**
         * @return The root entity.
         */
        Entity* GetRoot() const;

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

        /// Get a json file representing the root.
        /**
         * @return The json file representing the root.
         */
        Json::Value GetSaveJson() const;

        /// Load the world from file.
        /**
         * @param filename The name of the file.
         */
        void Load(const std::string& filename);

        /// Load the world from a json.
        /**
         * @param node The json containing a scene to load.
         */
        void Load(const Json::Value& node);

    private:
        // Copy constructor.
        World(World& world) = delete;

        // List of all entities in this world.
        std::vector<Entity*> entities;
        Entity* root = nullptr;

        // Entities registered for update event.
        std::vector<Entity*> updateEntities;
};
