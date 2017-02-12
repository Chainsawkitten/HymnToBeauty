#pragma once

#include <string>
#include <vector>

class asIScriptEngine;
class World;
class Entity;

/// Handles scripting.
class ScriptManager {
    friend class Hub;
    
    public:
        /// Build a script that can later be run.
        /**
         * @param name Name of the script to build.
         */
        void BuildScript(const std::string& name);
        
        /// Update all script components in the world.
        /**
         * @param world The world to update.
         */
        void Update(World& world);
        
        /// Register an entity to recieve update callbacks.
        /**
        * @param entity %Entity to register.
        * @todo Fix so registered entities can be removed.
        */
        void RegisterUpdate(Entity* entity);

        /// Register the input enum.
        void RegisterInput();

        /// The entity currently being executed.
        Entity* currentEntity;
        
    private:
        ScriptManager();
        ~ScriptManager();
        ScriptManager(ScriptManager const&) = delete;
        void operator=(ScriptManager const&) = delete;
        
        void CallScript(Entity* entity, const std::string& functionName);
        
        asIScriptEngine* engine;
        
        std::vector<Entity*> updateEntities;
};
