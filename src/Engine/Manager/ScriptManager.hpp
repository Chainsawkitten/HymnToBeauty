#pragma once

#include <string>
#include <vector>

class asIScriptEngine;
class asIScriptContext;
class asITypeInfo;
class World;
class Entity;
class ScriptFile;
namespace Component {
    class Script;
}

/// Handles scripting.
class ScriptManager {
    friend class Hub;
        
    public:
        /// Build a script in the script folder that can later be run.
        /**
         * @param name Name of the script to build.
         */
        void BuildScript(const std::string& name);
        
        /// Build all scripts in the script folder.
        void BuildAllScripts();
        
        /// Update all script entities in the game world.
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
        
        /// Send a message to an entity.
        /**
         * @param recipient The entity to receive the message.
         * @param type The type of message to send.
         */
        void SendMessage(Entity* recipient, int type);
        
        /// The entity currently being executed.
        Entity* currentEntity;
        
    private:
        struct Message {
            Entity* recipient;
            int type;
        };
        
        ScriptManager();
        ~ScriptManager();
        ScriptManager(ScriptManager const&) = delete;
        void operator=(ScriptManager const&) = delete;
        
        void CreateInstance(Component::Script* script);
        void CallScript(Component::Script* script, const std::string& functionName);
        void LoadScriptFile(const char* fileName, std::string& script);
        void ExecuteCall(asIScriptContext* context);
        asITypeInfo* GetClass(const std::string& moduleName, const std::string& className);
        
        asIScriptEngine* engine;
        
        std::vector<Entity*> updateEntities;
        std::vector<Message> messages;
};
