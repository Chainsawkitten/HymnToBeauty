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
class Sprite;
} // namespace Component
class TextureAsset;

/// Handles rendering the world.
class RenderManager {
    friend class Hub;

  public:
    /// Configuration for visualizing debug information.
    struct DebugConfiguration {
        /// Whether to show sound sources.
        bool showSoundSources;

        /// Texture to show sound sources with.
        Video::Texture2D* soundSourceTexture;

        /// Whether to show light sources.
        bool showLightSources;

        /// Texture to show light sources with.
        Video::Texture2D* lightTexture;

        /// Whether to show cameras.
        bool showCameras;

        /// Texture to show cameras with.
        Video::Texture2D* cameraTexture;

        /// Whether to show physics volumes.
        bool showPhysics;

        /// Whether to show light culling volumes.
        bool showLightVolumes;

        /// Whether to use full ambient instead of lighting the scene.
        bool unlit;
    };

    /// Render world containing entities.
    /**
     * @param world Contains a bunch of entities.
     * @param debugConfiguration Configuration for visualizing debug information.
     * @param cameraEntity Camera through which to render (or first camera in the world if nullptr).
     */
    void Render(World& world, const DebugConfiguration& debugConfiguration, Entity* cameraEntity = nullptr);

    /// Create directional light component.
    /**
     * @return The created component.
     */
    Component::DirectionalLight* CreateDirectionalLight();

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

    /// Get all spot light components.
    /**
     * @return All spot light components.
     */
    const std::vector<Component::SpotLight*>& GetSpotLights() const;

    /// Create sprite component.
    /**
     * @return The created component.
     */
    Component::Sprite* CreateSprite();

    /// Get all sprite components.
    /**
     * @return All sprite components.
     */
    const std::vector<Component::Sprite*>& GetSprites() const;

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
    void AddCameras(Video::RenderScene& renderScene, const glm::uvec2& windowSize);

    void AddLights(Video::RenderScene& renderScene, bool lighting, bool showLightVolumes);
    void AddWorldLights(Video::RenderScene& renderScene, bool showLightVolumes);
    void AddAmbientLight(Video::RenderScene& renderScene);

    void AddMeshes(Video::RenderScene& renderScene);

    void AddEditorEntities(Video::RenderScene& renderScene, const DebugConfiguration& debugConfiguration);

    void AddDebugShapes(Video::RenderScene& renderScene);

    void AddSprites(Video::RenderScene& renderScene);

    Video::Renderer* renderer;

    // Components.
    ComponentContainer<Component::DirectionalLight> directionalLights;
    ComponentContainer<Component::Camera> cameras;
    ComponentContainer<Component::Material> materials;
    ComponentContainer<Component::Mesh> meshes;
    ComponentContainer<Component::PointLight> pointLights;
    ComponentContainer<Component::SpotLight> spotLights;
    ComponentContainer<Component::Sprite> sprites;

    uint8_t textureReduction = 0;
};
