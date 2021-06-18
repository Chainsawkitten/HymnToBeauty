#pragma once

#include <glm/glm.hpp>
#include "../Entity/ComponentContainer.hpp"
#include <string>

namespace Video {
class Renderer;
class LowLevelRenderer;
class Texture2D;
} // namespace Video
class World;
class Entity;
namespace Component {
class DirectionalLight;
class Lens;
class Material;
class Mesh;
class PointLight;
class SpotLight;
} // namespace Component
namespace Json {
class Value;
}
class TextureAsset;

/// Handles rendering the world.
class RenderManager {
    friend class Hub;

  public:
    /// Render world containing entities.
    /**
     * @param world Contains a bunch of entities.
     * @param soundSources Whether to show sound sources.
     * @param lightSources Whether to show light sources.
     * @param cameras Whether to show cameras.
     * @param physics Whether to show physics volumes.
     * @param camera Camera through which to render (or first camera in the world if nullptr).
     * @param lighting Whether to light the scene (otherwise full ambient is used).
     * @param lightVolumes Whether to show light culling volumes.
     */
    void Render(World& world, bool soundSources = true, bool lightSources = true, bool cameras = true, bool physics = true, Entity* camera = nullptr, bool lighting = true, bool lightVolumes = false);

    /// Updates the buffers to fit the current screen size.
    void UpdateBufferSize();

    /// Create directional light component.
    /**
     * @return The created component.
     */
    Component::DirectionalLight* CreateDirectionalLight();

    /// Create directional light component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::DirectionalLight* CreateDirectionalLight(const Json::Value& node);

    /// Get all directional light components.
    /**
     * @return All directional light components.
     */
    const std::vector<Component::DirectionalLight*>& GetDirectionalLights() const;

    /// Create lens component.
    /**
     * @return The created component.
     */
    Component::Lens* CreateLens();

    /// Create lens component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::Lens* CreateLens(const Json::Value& node);

    /// Get all lens components.
    /**
     * @return All lens components.
     */
    const std::vector<Component::Lens*>& GetLenses() const;

    /// Create material component.
    /**
     * @return The created component.
     */
    Component::Material* CreateMaterial();

    /// Create material component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::Material* CreateMaterial(const Json::Value& node);

    /// Get all material components.
    /**
     * @return All material components.
     */
    const std::vector<Component::Material*>& GetMaterials() const;

    /// Create mesh component.
    /**
     * @return The created component.
     */
    Component::Mesh* CreateMesh();

    /// Create mesh component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::Mesh* CreateMesh(const Json::Value& node);

    /// Get all mesh components.
    /**
     * @return All mesh components.
     */
    const std::vector<Component::Mesh*>& GetMeshes() const;

    /// Create point light component.
    /**
     * @return The created component.
     */
    Component::PointLight* CreatePointLight();

    /// Create point light component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::PointLight* CreatePointLight(const Json::Value& node);

    /// Get all point light components.
    /**
     * @return All point light components.
     */
    const std::vector<Component::PointLight*>& GetPointLights() const;

    /// Create spot light component.
    /**
     * @return The created component.
     */
    Component::SpotLight* CreateSpotLight();

    /// Create spot light component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::SpotLight* CreateSpotLight(const Json::Value& node);

    /// Get all spot light components.
    /**
     * @return All spot light components.
     */
    const std::vector<Component::SpotLight*>& GetSpotLights() const;

    /// Remove all killed components.
    void ClearKilledComponents();

    /// Set gamma correction.
    /**
     * @param gamma Gamma to apply.
     */
    void SetGamma(float gamma);

    /// Get gamma correction.
    /**
     * @return gamma Gamma value.
     */
    float GetGamma() const;

    /// Set whether dithering is applied.
    /**
     * @param ditherApply Whether to apply dithering.
     */
    void SetDitherApply(bool ditherApply);

    /// Get whether dithering is applied.
    /**
     * @return Whether dithering is applied.
     */
    bool GetDitherApply() const;

    /// Get the number of lights currently being rendered.
    /**
     * @return Then number of lights being rendered.
     */
    unsigned int GetLightCount() const;

    /// Get the renderer.
    Video::Renderer* GetRenderer();

  private:
    explicit RenderManager(Video::Renderer* renderer);
    ~RenderManager();
    RenderManager(RenderManager const&) = delete;
    void operator=(RenderManager const&) = delete;

    void RenderWorldEntities(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lighting, float cameraNear, float cameraFar, bool lightVolumes);

    void RenderEditorEntities(World& world, bool soundSources, bool lightSources, bool cameras, bool physics, const glm::vec3& position, const glm::vec3& up, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    void LightWorld(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float zNear, float zFar, bool lightVolumes);
    void LightAmbient(const glm::mat4& projectionMatrix, float zNear, float zFar);

    void LoadTexture(TextureAsset*& texture, const std::string& name);

    Video::Renderer* renderer;

    // Editor entity textures.
    Video::Texture2D* lightTexture;
    Video::Texture2D* soundSourceTexture;
    Video::Texture2D* cameraTexture;

    // Components.
    ComponentContainer<Component::DirectionalLight> directionalLights;
    ComponentContainer<Component::Lens> lenses;
    ComponentContainer<Component::Material> materials;
    ComponentContainer<Component::Mesh> meshes;
    ComponentContainer<Component::PointLight> pointLights;
    ComponentContainer<Component::SpotLight> spotLights;

    uint8_t textureReduction = 0;
    unsigned int lightCount = 0;
};
