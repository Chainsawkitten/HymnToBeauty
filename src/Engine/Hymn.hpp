#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include <glm/glm.hpp>
#include "Manager/RenderManager.hpp"
#include "Entity/World.hpp"

class TextureAsset;
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
     * @param soundSources Whether to show sound sources.
     * @param particleEmitters Whether to show particle emitters.
     * @param lightSources Whether to show light sources.
     * @param cameras Whether to show cameras.
     * @param physics Whether to show physics volumes.
     * @param lighting Whether to light the world (otherwise full ambient is used).
     * @param lightVolumes Whether to show light culling volumes.
     */
    void Render(Entity* camera = nullptr, bool soundSources = false, bool particleEmitters = false, bool lightSources = false, bool cameras = false, bool physics = false, bool lighting = true, bool lightVolumes = false);

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

    /// Default albedo texture.
    TextureAsset* defaultAlbedo;

    /// Default normal texture.
    TextureAsset* defaultNormal;

    /// Default metallic texture.
    TextureAsset* defaultMetallic;

    /// Default roughness texture.
    TextureAsset* defaultRoughness;

    /// Filter settings.
    struct FilterSettings {
        /// Whether to enable color.
        bool colorFilterApply = false;

        /// The color to blend with.
        glm::vec3 colorFilterColor = glm::vec3(1.0f, 1.0f, 1.0f);

        /// Whether to enable fog.
        bool fogApply = false;

        /// Fog density.
        float fogDensity = 0.01f;

        /// Fog color.
        glm::vec3 fogColor = glm::vec3(1.0f, 1.0f, 1.0f);

        /// Whether to enable FXAA.
        bool fxaa = true;

        /// Whether to enable dithering.
        bool ditherApply = true;

        /// Gamma correction value.
        float gamma = 2.2f;
    };

    /// Filter settings.
    FilterSettings filterSettings;

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
