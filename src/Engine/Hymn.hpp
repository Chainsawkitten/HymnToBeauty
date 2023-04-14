#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include "Entity/World.hpp"
#include <Engine/Manager/RenderManager.hpp>

class ScriptFile;

/// A hymn to beauty.
class ActiveHymn {
    friend ActiveHymn& Hymn();

  public:
    /// Clear the hymn of all properties.
    void Clear();

    /// Get the path where the hymn is saved.
    /**
     * @return The hymn's path.
     */
    const std::string& GetPath() const;

    /// Set the path where the hymn is saved.
    /**
     * @param path New path.
     */
    void SetPath(const std::string& path);

    /// Gets the path to the hymn file.
    /**
     * @return The full path.
     */
    std::string GetSavePath() const;

    /// Save the hymn.
    void Save() const;

    /// Load a hymn.
    /**
     * @param path Path to the saved hymn.
     */
    void Load(const std::string& path);

    /// Convert the hymn to Json.
    /**
     * @return The hymn as a Json.
     */
    Json::Value ToJson() const;

    /// Convert a Json to a Hymn.
    /**
     * @param root The Json file to load.
     */
    void FromJson(Json::Value root);

    /// Update the world.
    /**
     * @param deltaTime Time since last frame (in seconds).
     */
    void Update(float deltaTime);

    /// Render the world.
    /**
     * @param debugConfiguration Configuration for visualizing debug information.
     * @param camera Camera through which to render (or first camera in the world if nullptr).
     */
    void Render(const RenderManager::DebugConfiguration& debugConfiguration, Entity* camera = nullptr);

    /// Find entity via GUID.
    /**
     * @param GUID The Unique Identifier for what entity you want to find.
     * @return Entity found or nullptr if entity with this param does not exist.
     */
    static Entity* GetEntityByGUID(unsigned int GUID);

    /// Scene to start when playing the hymn.
    std::string startupScene;

    /// The game world.
    World world;

    /// The id of the next entity to create.
    unsigned int entityNumber = 1U;

    /// Scripts.
    std::vector<ScriptFile*> scripts;

    /// The id of the next script to create.
    unsigned int scriptNumber = 0U;

    /// Whether to restart the hymn
    bool restart = false;

    /// Recently saved state of the world.
    Json::Value saveStateWorld;

    /// Recently saved state of the hymn.
    Json::Value saveStateHymn;

    /// The name of the hymn.
    std::string name;

  private:
    static ActiveHymn& GetInstance();

    ActiveHymn();
    ActiveHymn(ActiveHymn const&) = delete;
    void operator=(ActiveHymn const&) = delete;

    std::string path = "";
};

/// Get the active hymn.
/**
 * @return The %ActiveHymn instance.
 */
ActiveHymn& Hymn();
