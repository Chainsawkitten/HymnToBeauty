#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include <glm/glm.hpp>
#include "Entity/World.hpp"

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
     * @param camera Camera through which to render (or first camera in the world if nullptr).
     * @param showSoundSources Whether to show sound sources.
     * @param showLightSources Whether to show light sources.
     * @param showCameras Whether to show cameras.
     * @param showPhysics Whether to show physics volumes.
     * @param lighting Whether to light the world (otherwise full ambient is used).
     * @param showLightVolumes Whether to show light culling volumes.
     */
    void Render(Entity* camera = nullptr, bool showSoundSources = false, bool showLightSources = false, bool showCameras = false, bool showPhysics = false, bool lighting = true, bool showLightVolumes = false);

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
