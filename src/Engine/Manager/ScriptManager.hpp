#pragma once

#include <string>

class asIScriptEngine;
class Scene;
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
        
        /// Update all script components in a scene.
        /**
         * @param scene The scene to update.
         */
        void Update(Scene& scene);
        
        /// The entity currently being executed.
        Entity* currentEntity;
        
    private:
        ScriptManager();
        ~ScriptManager();
        ScriptManager(ScriptManager const&) = delete;
        void operator=(ScriptManager const&) = delete;
        
        asIScriptEngine* engine;
};
