#pragma once

#include <glm/glm.hpp>
#include "../Entity/ComponentContainer.hpp"
#include <string>

namespace Video {
class Renderer;
struct RenderScene;
class Texture2D;
} // namespace Video
class World;
class Entity;
namespace Component {
class DirectionalLight;
class Camera;
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
     * @param showSoundSources Whether to show sound sources.
     * @param showLightSources Whether to show light sources.
     * @param showCameras Whether to show cameras.
     * @param showPhysics Whether to show physics volumes.
     * @param cameraEntity Camera through which to render (or first camera in the world if nullptr).
     * @param lighting Whether to light the scene (otherwise full ambient is used).
     * @param lightVolumes Whether to show light culling volumes.
     */
    void Render(World& world, bool showSoundSources = true, bool showLightSources = true, bool showCameras = true, bool showPhysics = true, Entity* cameraEntity = nullptr, bool lighting = true, bool lightVolumes = false);

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

    /// Create camera component.
    /**
     * @return The created component.
     */
    Component::Camera* CreateCamera();

    /// Create camera component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::Camera* CreateCamera(const Json::Value& node);

    /// Get all camera components.
    /**
     * @return All camera components.
     */
    const std::vector<Component::Camera*>& GetCameras() const;

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

    /// Get the renderer.
    Video::Renderer* GetRenderer();

  private:
    explicit RenderManager(Video::Renderer* renderer);
    ~RenderManager();
    RenderManager(RenderManager const&) = delete;
    void operator=(RenderManager const&) = delete;

    void AddCamera(Video::RenderScene& renderScene, const Component::Camera& camera, const glm::uvec2& windowSize);

    void AddLights(Video::RenderScene& renderScene, bool lighting, bool showLightVolumes);
    void AddWorldLights(Video::RenderScene& renderScene, bool showLightVolumes);
    void AddAmbientLight(Video::RenderScene& renderScene);

    void AddMeshes(Video::RenderScene& renderScene);

    void AddEditorEntities(Video::RenderScene& renderScene, bool showSoundSources, bool showLightSources, bool showCameras, bool showPhysics);

    void AddDebugShapes(Video::RenderScene& renderScene);

    void LoadTexture(TextureAsset*& texture, const std::string& name);

    Video::Renderer* renderer;

    // Editor entity textures.
    Video::Texture2D* lightTexture;
    Video::Texture2D* soundSourceTexture;
    Video::Texture2D* cameraTexture;

    // Components.
    ComponentContainer<Component::DirectionalLight> directionalLights;
    ComponentContainer<Component::Camera> cameras;
    ComponentContainer<Component::Material> materials;
    ComponentContainer<Component::Mesh> meshes;
    ComponentContainer<Component::PointLight> pointLights;
    ComponentContainer<Component::SpotLight> spotLights;

    uint8_t textureReduction = 0;
};
