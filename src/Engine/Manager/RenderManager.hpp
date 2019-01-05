#pragma once

#include <glm/glm.hpp>
#include "../Entity/ComponentContainer.hpp"
#include <string>

namespace Video {
class Renderer;
class RenderSurface;
class TexturePNG;
class ShadowPass;
} // namespace Video
class World;
class Entity;
namespace Component {
class AnimationController;
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
    /// Types of displays to render.
    enum DISPLAY { MONITOR, HMD };

    /// Render world containing entities.
    /**
     * @param world Contains a bunch of entities.
     * @param targetDisplay Display type to render.
     * @param soundSources Whether to show sound sources.
     * @param particleEmitters Whether to show particle emitters.
     * @param lightSources Whether to show light sources.
     * @param cameras Whether to show cameras.
     * @param physics Whether to show physics volumes.
     * @param camera Camera through which to render (or first camera in the world if nullptr).
     * @param lighting Whether to light the scene (otherwise full ambient is used).
     * @param lightVolumes Whether to show light culling volumes.
     */
    void Render(World& world, DISPLAY targetDisplay, bool soundSources = true, bool particleEmitters = true, bool lightSources = true, bool cameras = true, bool physics = true, Entity* camera = nullptr, bool lighting = true, bool lightVolumes = false);

    /// Update all the animations in the scene.
    /**
     * @param deltaTime Time between frames.
     */
    void UpdateAnimations(float deltaTime);

    /// Updates the buffers to fit the current screen size.
    void UpdateBufferSize();

    /// Create animation component.
    /**
     * @return The created component.
     */
    Component::AnimationController* CreateAnimation();

    /// Create animation component.
    /**
     * @param node Json node to load the component from.
     * @return The created component.
     */
    Component::AnimationController* CreateAnimation(const Json::Value& node);

    /// Get all animation controller components.
    /**
     * @return All animation controller components.
     */
    const std::vector<Component::AnimationController*>& GetAnimations() const;

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

    /// Set whether fog is applied.
    /**
     * @param fogApply Whether to apply fog.
     */
    void SetFogApply(bool fogApply);

    /// Get whether fog is applied.
    /**
     * @return Whether fog is applied.
     */
    bool GetFogApply() const;

    /// Set fog density.
    /**
     * @param fogDensity Density of the fog.
     */
    void SetFogDensity(float fogDensity);

    /// Get fog density.
    /**
     * @return Density of the fog
     */
    float GetFogDensity() const;

    /// Set fog color.
    /**
     * @param fogColor Color of the fog.
     */
    void SetFogColor(const glm::vec3& fogColor);

    /// Get fog color.
    /**
     * @return Color of the fog.
     */
    glm::vec3 GetFogColor() const;

    /// Set whether color filter is applied.
    /**
     * @param colorFilterApply Whether to apply color filter.
     */
    void SetColorFilterApply(bool colorFilterApply);

    /// Get whether color filter is applied.
    /**
     * @return Whether color filter is applied.
     */
    bool GetColorFilterApply() const;

    /// Set color filer color.
    /**
     * @param colorFilterColor Color of the color filter.
     */
    void SetColorFilterColor(const glm::vec3& colorFilterColor);

    /// Get color filer color.
    /**
     * @return Color of the color filter.
     */
    glm::vec3 GetColorFilterColor() const;

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

    /// Set the level of texture reduction to use.
    /**
     * @param textureReduction Which mip-level to start loading.
     */
    void SetTextureReduction(uint16_t textureReduction);

    /// Get the level of texture reduction to use.
    /**
     * @return Which mip-level to start loading.
     */
    uint16_t GetTextureReduction() const;

    /// Get the number of lights currently being rendered.
    /**
     * @return Then number of lights being rendered.
     */
    unsigned int GetLightCount() const;

    /// Set the size of the shadow map.
    /**
     * @param shadowMapSize The size of the shadow map.
     */
    void SetShadowMapSize(unsigned int shadowMapSize);

  private:
    RenderManager();
    ~RenderManager();
    RenderManager(RenderManager const&) = delete;
    void operator=(RenderManager const&) = delete;

    void RenderWorldEntities(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface, bool lighting, float cameraNear, float cameraFar, bool lightVolumes);

    void RenderEditorEntities(World& world, bool soundSources, bool particleEmitters, bool lightSources, bool cameras, bool physics, const glm::vec3& position, const glm::vec3& up, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface);

    void LightWorld(const glm::mat4& viewMatrix, const glm::mat4& viewProjectionMatrix, bool lightVolumes);
    void LightAmbient();

    void LoadTexture(TextureAsset*& texture, const std::string& name);

    Video::Renderer* renderer;

    Video::ShadowPass* shadowPass;

    Video::RenderSurface* mainWindowRenderSurface;

    // Editor entity textures.
    Video::TexturePNG* particleEmitterTexture;
    Video::TexturePNG* lightTexture;
    Video::TexturePNG* soundSourceTexture;
    Video::TexturePNG* cameraTexture;

    // Components.
    ComponentContainer<Component::AnimationController> animationControllers;
    ComponentContainer<Component::DirectionalLight> directionalLights;
    ComponentContainer<Component::Lens> lenses;
    ComponentContainer<Component::Material> materials;
    ComponentContainer<Component::Mesh> meshes;
    ComponentContainer<Component::PointLight> pointLights;
    ComponentContainer<Component::SpotLight> spotLights;

    uint8_t textureReduction = 0;
    unsigned int lightCount = 0;
};
