#pragma once

#include <string>
#include <vector>

class asIScriptEngine;
class World;
class Entity;
class ScriptFile;

/// Handles scripting.
class ScriptManager {
    friend class Hub;
    
    public:

        /// Build a script¨in the script folder that can later be run.
        /**
         * @param name Name of the script to build.
         */
        void BuildScript(const std::string& name);
        
        /// Build a script that can later be run.
        /**
         * @param name The script to build.
         */
        void BuildSpecificScript(const std::string& path);

        /// Build all scripts in the script folder.
        void BuildAllScripts();

        ///Loads a script from a .as file into a string readable by the ASengine.
        void LoadScriptFile(const char* fileName, std::string& script);

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
        
        /// The entity currently being executed.
        Entity* currentEntity;
        
    private:
        ScriptManager();
        ~ScriptManager();
        ScriptManager(ScriptManager const&) = delete;
        void operator=(ScriptManager const&) = delete;
        
        void CallScript(Entity* entity, const std::string& functionName);
        void CallSpecificScript(ScriptFile* script, const std::string& functionName);

        asIScriptEngine* engine;
        
        std::vector<Entity*> updateEntities;
};
