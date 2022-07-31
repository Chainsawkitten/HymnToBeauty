#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include "../Entity/ComponentContainer.hpp"

class asIScriptEngine;
class asIScriptContext;
class asITypeInfo;
class World;
class Entity;
class ScriptFile;
namespace Component {
class Script;
}
namespace Utility {
class Window;
}

/// Handles scripting.
class ScriptManager {
    friend class Hub;

  public:
    /// Build a script that can later be run.
    /**
     * @param script Script to build.
     * @return The result, < 0 means it failed.
     */
    int BuildScript(ScriptFile* script);

    /// Build all scripts in the hymn.
    void BuildAllScripts();

    /// Fetches the properties from the script and fills the map.
    /**
     * @param script The script which map to update.
     */
    void FillPropertyMap(Component::Script* script);

    /// Fetches the functions from the script and fills the scriptfiles vector.
    /**
     * @param scriptFile The scriptfile which vector to update.
     */
    void FillFunctionVector(ScriptFile* scriptFile);

    /// Update all script entities in the game world.
    /**
     * @param world The world to update.
     * @param deltaTime Time since last frame (in seconds).
     */
    void Update(World& world, float deltaTime);

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
     * @param sender The entity that sent the message.
     * @param type The type of message to send.
     */
    void SendMessage(Entity* recipient, Entity* sender, int type);

    /// Create script component.
    /**
     * @return The created component.
     */
    Component::Script* CreateScript();

    /// Used to get the string identifier used to check if a property is a string.
    /**
     * @return The identifier of the string declaration.
     */
    int GetStringDeclarationID();

    /// Get all script components.
    /**
     * @return All script components.
     */
    const std::vector<Component::Script*>& GetScripts() const;

    /// Remove all killed components.
    void ClearKilledComponents();

    /// Execute a method on a script.
    /**
     * @param entity Target entity whose script will be run.
     * @param method Method to call.
     */
    void ExecuteScriptMethod(const Entity* entity, const std::string& method);

    /// The entity currently being executed.
    Entity* currentEntity;

    /// Get the set of entities with a script component that accepts
    /// update events.
    /**
     * @return Entities with script updates.
     */
    const std::vector<Entity*>& GetUpdateEntities();

  private:
    struct Message {
        Entity* recipient;
        Entity* sender;
        int type;
    };

    explicit ScriptManager(Utility::Window* window);
    ~ScriptManager();
    ScriptManager(ScriptManager const&) = delete;
    void operator=(ScriptManager const&) = delete;

    void CreateInstance(Component::Script* script);
    asIScriptContext* CreateContext();
    void CallMessageReceived(const Message& message);
    void CallUpdate(Entity* entity, float deltaTime);
    void LoadScriptFile(const char* fileName, std::string& script);
    void ExecuteCall(asIScriptContext* context, const std::string& scriptName);
    asITypeInfo* GetClass(const std::string& moduleName, const std::string& className);

    bool IsIntersect(Entity* checker, Entity* camera) const;

    asIScriptEngine* engine;

    std::vector<Entity*> updateEntities;
    std::vector<Message> messages;

    void GetBreakpoints(const ScriptFile* script);
    void ClearBreakpoints();
    std::map<std::string, std::set<int>> breakpoints;

    ComponentContainer<Component::Script> scripts;

    Utility::Window* window;
};
