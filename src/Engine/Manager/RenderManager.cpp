#include "RenderManager.hpp"

#include <Video/Buffer/FrameBuffer.hpp>
#include <Video/Renderer.hpp>
#include <Video/RenderSurface.hpp>
#include <Video/Buffer/ReadWriteTexture.hpp>
#include <Video/VideoErrorCheck.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "ParticleManager.hpp"
#include "SoundManager.hpp"
#include "PhysicsManager.hpp"
#include "DebugDrawingManager.hpp"
#include "Light.png.hpp"
#include "ParticleEmitter.png.hpp"
#include "SoundSource.png.hpp"
#include "Camera.png.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/AnimationController.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/Shape.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/SoundSource.hpp"
#include "../Geometry/Model.hpp"
#include "../Physics/Shape.hpp"
#include <Video/Geometry/Geometry3D.hpp>
#include "../Texture/TextureAsset.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <Video/Culling/Frustum.hpp>
#include <Video/Culling/AxisAlignedBoundingBox.hpp>
#include "../MainWindow.hpp"
#include <Video/Lighting/Light.hpp>
#include "../Hymn.hpp"
#include "../Util/Profiling.hpp"
#include "../Util/Json.hpp"
#include "../Util/GPUProfiling.hpp"
#include <Utility/Log.hpp>
#include <Video/ShadowPass.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Video/Texture/TexturePNG.hpp>

using namespace Component;

RenderManager::RenderManager() {
    renderer = new Video::Renderer();

    // Render surface for main window.
    mainWindowRenderSurface = new Video::RenderSurface(MainWindow::GetInstance()->GetSize());

    // Init shadowpass.
    shadowPass = new Video::ShadowPass();

    // Init textures.
    particleEmitterTexture = Managers().resourceManager->CreateTexturePNG(PARTICLEEMITTER_PNG, PARTICLEEMITTER_PNG_LENGTH);
    lightTexture = Managers().resourceManager->CreateTexturePNG(LIGHT_PNG, LIGHT_PNG_LENGTH);
    soundSourceTexture = Managers().resourceManager->CreateTexturePNG(SOUNDSOURCE_PNG, SOUNDSOURCE_PNG_LENGTH);
    cameraTexture = Managers().resourceManager->CreateTexturePNG(CAMERA_PNG, CAMERA_PNG_LENGTH);
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeTexturePNG(particleEmitterTexture);
    Managers().resourceManager->FreeTexturePNG(lightTexture);
    Managers().resourceManager->FreeTexturePNG(soundSourceTexture);
    Managers().resourceManager->FreeTexturePNG(cameraTexture);

    delete mainWindowRenderSurface;
    delete shadowPass;

    delete renderer;
}

void RenderManager::Render(World& world, bool soundSources, bool particleEmitters, bool lightSources, bool cameras, bool physics, Entity* camera, bool lighting, bool lightVolumes) {
    // Find camera entity.
    if (camera == nullptr) {
        for (Lens* lens : lenses.GetAll())
            camera = lens->entity;
    }

    if (camera != nullptr) {
        // Set image processing variables.
        renderer->SetGamma(Hymn().filterSettings.gamma);
        renderer->SetFogApply(Hymn().filterSettings.fogApply && lighting);
        renderer->SetFogDensity(Hymn().filterSettings.fogDensity);
        renderer->SetFogColor(Hymn().filterSettings.fogColor);
        renderer->SetColorFilterApply(Hymn().filterSettings.colorFilterApply);
        renderer->SetColorFilterColor(Hymn().filterSettings.colorFilterColor);
        renderer->SetDitherApply(Hymn().filterSettings.ditherApply);
        const bool fxaa = Hymn().filterSettings.fxaa;
        const glm::vec2 windowSize = MainWindow::GetInstance()->GetSize();

        // Render to surfaces.
        if (mainWindowRenderSurface != nullptr && windowSize.x > 0 && windowSize.y > 0) {
            renderer->SetFrameSize(mainWindowRenderSurface->GetSize());
            // Render main window.
            {
                PROFILE("Render main window");
                GPUPROFILE("Render main window", Video::Query::Type::TIME_ELAPSED);
                Lens* lens = camera->GetComponent<Lens>();
                const glm::mat4 projectionMatrix = lens->GetProjection(mainWindowRenderSurface->GetSize());
                const glm::mat4 viewMatrix = glm::inverse(camera->GetModelMatrix());
                const glm::vec3 position = camera->GetWorldPosition();
                const glm::vec3 up(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

                {
                    VIDEO_ERROR_CHECK("Render world entities");
                    PROFILE("Render world entities");
                    GPUPROFILE("Render world entities", Video::Query::Type::TIME_ELAPSED);

                    RenderWorldEntities(world, viewMatrix, projectionMatrix, mainWindowRenderSurface, lighting, lens->zNear, lens->zFar, lightVolumes);
                }

                {
                    PROFILE("Render debug entities");
                    GPUPROFILE("Render debug entities", Video::Query::Type::TIME_ELAPSED);

                    Managers().debugDrawingManager->Render(viewMatrix, projectionMatrix, mainWindowRenderSurface);
                }

                if (fxaa) {
                    PROFILE("Anti-aliasing(FXAA)");
                    GPUPROFILE("Anti-aliasing(FXAA)", Video::Query::Type::TIME_ELAPSED);
                    {
                        GPUPROFILE("Anti-aliasing(FXAA)", Video::Query::Type::SAMPLES_PASSED);
                        renderer->AntiAlias(mainWindowRenderSurface);
                    }
                }

                {
                    PROFILE("Render particles");
                    GPUPROFILE("Render particles", Video::Query::Type::TIME_ELAPSED);

                    mainWindowRenderSurface->GetShadingFrameBuffer()->BindWrite();
                    Managers().particleManager->RenderParticleSystem(viewMatrix, projectionMatrix);
                    mainWindowRenderSurface->GetShadingFrameBuffer()->Unbind();
                }

                if (soundSources || particleEmitters || lightSources || cameras || physics) {
                    PROFILE("Render editor entities");
                    GPUPROFILE("Render editor entities", Video::Query::Type::TIME_ELAPSED);

                    RenderEditorEntities(world, soundSources, particleEmitters, lightSources, cameras, physics, position, up, viewMatrix, projectionMatrix, mainWindowRenderSurface);
                }

                {
                    PROFILE("Present to back buffer");
                    GPUPROFILE("Present to back buffer", Video::Query::Type::TIME_ELAPSED);
                    {
                        GPUPROFILE("Present to back buffer", Video::Query::Type::SAMPLES_PASSED);
                        renderer->Present(mainWindowRenderSurface, windowSize);
                    }
                }
            }
        }
    }
}

void RenderManager::UpdateBufferSize() {
    delete mainWindowRenderSurface;
    mainWindowRenderSurface = new Video::RenderSurface(MainWindow::GetInstance()->GetSize());
}

void RenderManager::RenderWorldEntities(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface, bool lighting, float cameraNear, float cameraFar, bool lightVolumes) {
    // Render from camera.
    glm::mat4 lightViewMatrix;
    glm::mat4 lightProjection;

    for (Component::SpotLight* spotLight : spotLights.GetAll()) {
        if (spotLight->IsKilled() || !spotLight->entity->IsEnabled())
            continue;

        if (spotLight->shadow) {
            Entity* lightEntity = spotLight->entity;
            lightViewMatrix = glm::inverse(lightEntity->GetModelMatrix());
            lightProjection = glm::perspective(glm::radians(2.f * spotLight->coneAngle), 1.0f, 0.01f, spotLight->distance);
        }
    }

    // Camera matrices.
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    const std::vector<Mesh*>& meshComponents = meshes.GetAll();
    const std::vector<AnimationController*>& controllerComponents = animationControllers.GetAll();

    // Render shadows maps.
    {
        VIDEO_ERROR_CHECK("Render shadow meshes");
        PROFILE("Render shadow meshes");
        GPUPROFILE("Render shadow meshes", Video::Query::Type::TIME_ELAPSED);
        {
            GPUPROFILE("Render shadow meshes", Video::Query::Type::SAMPLES_PASSED);

            // Static meshes.
            renderer->PrepareStaticShadowRendering(lightViewMatrix, lightProjection, shadowPass->GetShadowID(), shadowPass->GetShadowMapSize(), shadowPass->GetDepthMapFbo());
            for (Mesh* mesh : meshComponents) {
                Entity* entity = mesh->entity;
                if (entity->IsKilled() || !entity->IsEnabled())
                    continue;

                if (mesh->geometry && mesh->geometry->GetIndexCount() != 0 && mesh->geometry->GetType() == Video::Geometry::Geometry3D::STATIC)
                    renderer->ShadowRenderStaticMesh(mesh->geometry, lightViewMatrix, lightProjection, entity->GetModelMatrix());
            }
            // Skin meshes.
            renderer->PrepareSkinShadowRendering(lightViewMatrix, lightProjection, shadowPass->GetShadowID(), shadowPass->GetShadowMapSize(), shadowPass->GetDepthMapFbo());
            for (AnimationController* controller : controllerComponents) {
                Entity* entity = controller->entity;
                if (entity->IsKilled() || !entity->IsEnabled())
                    continue;

                Mesh* mesh = entity->GetComponent<Mesh>();
                if (mesh && mesh->geometry && mesh->geometry->GetIndexCount() != 0 && mesh->geometry->GetType() == Video::Geometry::Geometry3D::SKIN)
                    renderer->ShadowRenderSkinMesh(mesh->geometry, lightViewMatrix, lightProjection, entity->GetModelMatrix(), controller->bones);
            }
        }
    }

    // Render z-pass meshes.
    renderer->StartRendering(renderSurface);
    renderSurface->GetDepthFrameBuffer()->BindWrite();
    {
        VIDEO_ERROR_CHECK("Render z-pass meshes");
        PROFILE("Render z-pass meshes");
        GPUPROFILE("Render z-pass meshes", Video::Query::Type::TIME_ELAPSED);
        {
            GPUPROFILE("Render z-pass meshes", Video::Query::Type::SAMPLES_PASSED);

            // Static meshes.
            renderer->PrepareStaticMeshDepthRendering(viewMatrix, projectionMatrix);
            for (Mesh* mesh : meshComponents) {
                Entity* entity = mesh->entity;
                if (entity->IsKilled() || !entity->IsEnabled())
                    continue;

                if (mesh->geometry && mesh->geometry->GetIndexCount() != 0 && mesh->geometry->GetType() == Video::Geometry::Geometry3D::STATIC)
                    if (entity->GetComponent<Material>() != nullptr)
                        renderer->DepthRenderStaticMesh(mesh->geometry, viewMatrix, projectionMatrix, entity->GetModelMatrix());
            }

            // Skin meshes.
            renderer->PrepareSkinMeshDepthRendering(viewMatrix, projectionMatrix);
            for (AnimationController* controller : controllerComponents) {
                Entity* entity = controller->entity;
                if (entity->IsKilled() || !entity->IsEnabled())
                    continue;

                Mesh* mesh = entity->GetComponent<Mesh>();
                if (mesh && mesh->geometry && mesh->geometry->GetIndexCount() != 0 && mesh->geometry->GetType() == Video::Geometry::Geometry3D::SKIN)
                    if (entity->GetComponent<Material>() != nullptr)
                        renderer->DepthRenderSkinMesh(mesh->geometry, viewMatrix, projectionMatrix, entity->GetModelMatrix(), controller->bones);
            }
        }
    }
    renderSurface->GetDepthFrameBuffer()->Unbind();

    // Lights.
    {
        VIDEO_ERROR_CHECK("Update lights");
        PROFILE("Update lights");
        GPUPROFILE("Update lights", Video::Query::Type::TIME_ELAPSED);

        if (lighting)
            // Cull lights and update light list.
            LightWorld(viewMatrix, viewProjectionMatrix, lightVolumes);
        else
            // Use full ambient light and ignore lights in the scene.
            LightAmbient();
    }

    // Render meshes.
    renderSurface->GetShadingFrameBuffer()->BindWrite();
    {
        VIDEO_ERROR_CHECK("Render meshes");
        PROFILE("Render meshes");
        GPUPROFILE("Render meshes", Video::Query::Type::TIME_ELAPSED);

        // Static meshes.
        {
            VIDEO_ERROR_CHECK("Static meshes");
            PROFILE("Static meshes");
            GPUPROFILE("Static meshes", Video::Query::Type::TIME_ELAPSED);
            {
                GPUPROFILE("Static meshes", Video::Query::Type::SAMPLES_PASSED);
                renderer->PrepareStaticMeshRendering(viewMatrix, projectionMatrix, cameraNear, cameraFar);
                for (Mesh* mesh : meshComponents) {
                    Entity* entity = mesh->entity;
                    if (entity->IsKilled() || !entity->IsEnabled())
                        continue;

                    if (mesh->geometry && mesh->geometry->GetIndexCount() != 0 && mesh->geometry->GetType() == Video::Geometry::Geometry3D::STATIC) {
                        Material* material = entity->GetComponent<Material>();
                        if (material != nullptr)
                            renderer->RenderStaticMesh(mesh->geometry, material->albedo->GetTexture(), material->normal->GetTexture(), material->metallic->GetTexture(), material->roughness->GetTexture(), entity->GetModelMatrix());
                    }
                }
            }
        }

        // Skin meshes.
        {
            VIDEO_ERROR_CHECK("Skin meshes");
            PROFILE("Skin meshes");
            GPUPROFILE("Skin meshes", Video::Query::Type::TIME_ELAPSED);
            {
                GPUPROFILE("Skin meshes", Video::Query::Type::SAMPLES_PASSED);
                renderer->PrepareSkinMeshRendering(viewMatrix, projectionMatrix, cameraNear, cameraFar);
                for (AnimationController* controller : controllerComponents) {
                    Entity* entity = controller->entity;
                    if (entity->IsKilled() || !entity->IsEnabled())
                        continue;

                    Mesh* mesh = entity->GetComponent<Mesh>();
                    if (mesh && mesh->geometry && mesh->geometry->GetIndexCount() != 0 && mesh->geometry->GetType() == Video::Geometry::Geometry3D::SKIN) {
                        Material* material = entity->GetComponent<Material>();
                        if (material)
                            renderer->RenderSkinMesh(mesh->geometry, material->albedo->GetTexture(), material->normal->GetTexture(), material->metallic->GetTexture(), material->roughness->GetTexture(), entity->GetModelMatrix(), controller->bones);
                    }
                }
            }
        }
    }
    renderSurface->GetShadingFrameBuffer()->Unbind();
}

void RenderManager::UpdateAnimations(float deltaTime) {
    // Update all enabled animation controllers.
    for (Component::AnimationController* animationController : animationControllers.GetAll()) {
        if (animationController->IsKilled() || !animationController->entity->IsEnabled())
            continue;

        animationController->UpdateAnimation(deltaTime);
    }
}

void RenderManager::RenderEditorEntities(World& world, bool soundSources, bool particleEmitters, bool lightSources, bool cameras, bool physics, const glm::vec3& position, const glm::vec3& up, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface) {
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

    renderSurface->GetShadingFrameBuffer()->BindWrite();
    renderer->PrepareRenderingIcons(viewProjectionMatrix, position, up);

    // Render sound sources.
    if (soundSources) {
        for (SoundSource* soundSource : Managers().soundManager->GetSoundSources())
            renderer->RenderIcon(soundSource->entity->GetWorldPosition(), soundSourceTexture);
    }

    // Render light sources.
    if (lightSources) {
        for (DirectionalLight* light : directionalLights.GetAll())
            renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);

        for (PointLight* light : pointLights.GetAll())
            renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);

        for (SpotLight* light : spotLights.GetAll())
            renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
    }

    // Render cameras.
    if (cameras) {
        for (Lens* lens : lenses.GetAll())
            renderer->RenderIcon(lens->entity->GetWorldPosition(), cameraTexture);
    }

    renderer->StopRenderingIcons();
    renderSurface->GetShadingFrameBuffer()->Unbind();

    // Render physics.
    if (physics) {
        for (Component::Shape* shapeComp : Managers().physicsManager->GetShapeComponents()) {
            const ::Physics::Shape& shape = *shapeComp->GetShape();
            if (shape.GetKind() == ::Physics::Shape::Kind::Sphere) {
                Managers().debugDrawingManager->AddSphere(shapeComp->entity->GetWorldPosition(), shape.GetSphereData()->radius, glm::vec3(1.0f, 1.0f, 1.0f));
            } else if (shape.GetKind() == ::Physics::Shape::Kind::Plane) {
                glm::vec3 normal = shapeComp->entity->GetModelMatrix() * glm::vec4(shape.GetPlaneData()->normal, 0.0f);
                Managers().debugDrawingManager->AddPlane(shapeComp->entity->GetWorldPosition(), normal, glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
            } else if (shape.GetKind() == ::Physics::Shape::Kind::Box) {
                glm::vec3 dimensions(shape.GetBoxData()->width, shape.GetBoxData()->height, shape.GetBoxData()->depth);
                glm::vec3 position = shapeComp->entity->GetWorldPosition();
                glm::quat orientation = shapeComp->entity->GetWorldOrientation();
                glm::mat4 transformationMatrix = glm::translate(glm::mat4(), position) * glm::toMat4(orientation);
                Managers().debugDrawingManager->AddCuboid(dimensions, transformationMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
            } else if (shape.GetKind() == ::Physics::Shape::Kind::Cylinder) {
                glm::vec3 position = shapeComp->entity->GetWorldPosition();
                glm::quat orientation = shapeComp->entity->GetWorldOrientation();
                glm::mat4 transformationMatrix = glm::translate(glm::mat4(), position) * glm::toMat4(orientation);
                Managers().debugDrawingManager->AddCylinder(shape.GetCylinderData()->radius, shape.GetCylinderData()->length, transformationMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
            } else if (shape.GetKind() == ::Physics::Shape::Kind::Cone) {
                glm::vec3 position = shapeComp->entity->GetWorldPosition();
                glm::quat orientation = shapeComp->entity->GetWorldOrientation();
                glm::mat4 transformationMatrix = glm::translate(glm::mat4(), position) * glm::toMat4(orientation);
                Managers().debugDrawingManager->AddCone(shape.GetConeData()->radius, shape.GetConeData()->height, transformationMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
    }
}

Component::AnimationController* RenderManager::CreateAnimation() {
    return animationControllers.Create();
}

Component::AnimationController* RenderManager::CreateAnimation(const Json::Value& node) {
    Component::AnimationController* animationController = animationControllers.Create();

    std::string skeletonName = node.get("skeleton", "").asString();
    if (!skeletonName.empty())
        animationController->skeleton = Managers().resourceManager->CreateSkeleton(skeletonName);

    std::string controllerName = node.get("animationController", "").asString();
    if (!controllerName.empty())
        animationController->controller = Managers().resourceManager->CreateAnimationController(controllerName);

    return animationController;
}

const std::vector<Component::AnimationController*>& RenderManager::GetAnimations() const {
    return animationControllers.GetAll();
}

Component::DirectionalLight* RenderManager::CreateDirectionalLight() {
    return directionalLights.Create();
}

Component::DirectionalLight* RenderManager::CreateDirectionalLight(const Json::Value& node) {
    Component::DirectionalLight* directionalLight = directionalLights.Create();

    // Load values from Json node.
    directionalLight->color = Json::LoadVec3(node["color"]);
    directionalLight->ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();

    return directionalLight;
}

const std::vector<Component::DirectionalLight*>& RenderManager::GetDirectionalLights() const {
    return directionalLights.GetAll();
}

Component::Lens* RenderManager::CreateLens() {
    return lenses.Create();
}

Component::Lens* RenderManager::CreateLens(const Json::Value& node) {
    Component::Lens* lens = lenses.Create();

    // Load values from Json node.
    lens->fieldOfView = node.get("fieldOfView", 45.f).asFloat();
    lens->zNear = node.get("zNear", 0.1f).asFloat();
    lens->zFar = node.get("zFar", 100.f).asFloat();

    return lens;
}

const std::vector<Component::Lens*>& RenderManager::GetLenses() const {
    return lenses.GetAll();
}

Component::Material* RenderManager::CreateMaterial() {
    return materials.Create();
}

Component::Material* RenderManager::CreateMaterial(const Json::Value& node) {
    Component::Material* material = materials.Create();

    // Load values from Json node.
    LoadTexture(material->albedo, node.get("albedo", "").asString());
    LoadTexture(material->normal, node.get("normal", "").asString());
    LoadTexture(material->metallic, node.get("metallic", "").asString());
    LoadTexture(material->roughness, node.get("roughness", "").asString());

    return material;
}

const std::vector<Component::Material*>& RenderManager::GetMaterials() const {
    return materials.GetAll();
}

Component::Mesh* RenderManager::CreateMesh() {
    return meshes.Create();
}

Component::Mesh* RenderManager::CreateMesh(const Json::Value& node) {
    Component::Mesh* mesh = meshes.Create();

    // Load values from Json node.
    std::string meshName = node.get("model", "").asString();
    mesh->geometry = Managers().resourceManager->CreateModel(meshName);

    return mesh;
}

const std::vector<Component::Mesh*>& RenderManager::GetMeshes() const {
    return meshes.GetAll();
}

Component::PointLight* RenderManager::CreatePointLight() {
    return pointLights.Create();
}

Component::PointLight* RenderManager::CreatePointLight(const Json::Value& node) {
    Component::PointLight* pointLight = pointLights.Create();

    // Load values from Json node.
    pointLight->color = Json::LoadVec3(node["color"]);
    pointLight->attenuation = node.get("attenuation", 1.f).asFloat();
    pointLight->intensity = node.get("intensity", 1.f).asFloat();
    pointLight->distance = node.get("distance", 1.f).asFloat();

    return pointLight;
}

const std::vector<Component::PointLight*>& RenderManager::GetPointLights() const {
    return pointLights.GetAll();
}

Component::SpotLight* RenderManager::CreateSpotLight() {
    return spotLights.Create();
}

Component::SpotLight* RenderManager::CreateSpotLight(const Json::Value& node) {
    Component::SpotLight* spotLight = spotLights.Create();

    // Load values from Json node.
    spotLight->color = Json::LoadVec3(node["color"]);
    spotLight->ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
    spotLight->attenuation = node.get("attenuation", 1.f).asFloat();
    spotLight->intensity = node.get("intensity", 1.f).asFloat();
    spotLight->coneAngle = node.get("coneAngle", 15.f).asFloat();
    spotLight->shadow = node.get("shadow", false).asBool();
    spotLight->distance = node.get("distance", 1.f).asFloat();

    return spotLight;
}

const std::vector<Component::SpotLight*>& RenderManager::GetSpotLights() const {
    return spotLights.GetAll();
}

void RenderManager::ClearKilledComponents() {
    animationControllers.ClearKilled();
    directionalLights.ClearKilled();
    lenses.ClearKilled();
    materials.ClearKilled();
    meshes.ClearKilled();
    pointLights.ClearKilled();
    spotLights.ClearKilled();
}

void RenderManager::SetGamma(float gamma) {
    Hymn().filterSettings.gamma = gamma;
}

float RenderManager::GetGamma() const {
    return Hymn().filterSettings.gamma;
}

void RenderManager::SetFogApply(bool fogApply) {
    Hymn().filterSettings.fogApply = fogApply;
}

bool RenderManager::GetFogApply() const {
    return Hymn().filterSettings.fogApply;
}

void RenderManager::SetFogDensity(float fogDensity) {
    Hymn().filterSettings.fogDensity = fogDensity;
}

float RenderManager::GetFogDensity() const {
    return Hymn().filterSettings.fogDensity;
}

void RenderManager::SetFogColor(const glm::vec3& fogColor) {
    Hymn().filterSettings.fogColor = fogColor;
}

glm::vec3 RenderManager::GetFogColor() const {
    return Hymn().filterSettings.fogColor;
}

void RenderManager::SetColorFilterApply(bool colorFilterApply) {
    Hymn().filterSettings.colorFilterApply = colorFilterApply;
}

bool RenderManager::GetColorFilterApply() const {
    return Hymn().filterSettings.colorFilterApply;
}

void RenderManager::SetColorFilterColor(const glm::vec3& colorFilterColor) {
    Hymn().filterSettings.colorFilterColor = colorFilterColor;
}

glm::vec3 RenderManager::GetColorFilterColor() const {
    return Hymn().filterSettings.colorFilterColor;
}

void RenderManager::SetDitherApply(bool ditherApply) {
    Hymn().filterSettings.ditherApply = ditherApply;
}

bool RenderManager::GetDitherApply() const {
    return Hymn().filterSettings.ditherApply;
}

unsigned int RenderManager::GetLightCount() const {
    return lightCount;
}

void RenderManager::SetShadowMapSize(unsigned int shadowMapSize) {
    shadowPass->SetShadowMapSize(shadowMapSize);
}

void RenderManager::LightWorld(const glm::mat4& viewMatrix, const glm::mat4& viewProjectionMatrix, bool lightVolumes) {
    std::vector<Video::Light> lights;

    Video::AxisAlignedBoundingBox aabb(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    // Add all directional lights.
    for (Component::DirectionalLight* directionalLight : directionalLights.GetAll()) {
        if (directionalLight->IsKilled() || !directionalLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = directionalLight->entity;
        glm::vec4 direction(glm::vec4(lightEntity->GetDirection(), 0.f));
        Video::Light light;
        light.position = viewMatrix * -direction;
        light.intensities = directionalLight->color;
        light.attenuation = 1.f;
        light.ambientCoefficient = directionalLight->ambientCoefficient;
        light.coneAngle = 0.f;
        light.direction = glm::vec3(0.f, 0.f, 0.f);
        light.shadow = 0.f;
        light.distance = 0.f;
        lights.push_back(light);
    }

    // Add all spot lights.
    for (Component::SpotLight* spotLight : spotLights.GetAll()) {
        if (spotLight->IsKilled() || !spotLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = spotLight->entity;
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->GetWorldPosition()) * glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f) * spotLight->distance);

        // TMPTODO
        Video::Frustum frustum(viewProjectionMatrix * modelMat);
        if (frustum.Collide(aabb)) {
            if (lightVolumes)
                Managers().debugDrawingManager->AddSphere(lightEntity->GetWorldPosition(), spotLight->distance, glm::vec3(1.0f, 1.0f, 1.0f));

            glm::vec4 direction(viewMatrix * glm::vec4(lightEntity->GetDirection(), 0.f));
            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            Video::Light light;
            light.position = viewMatrix * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0));
            light.intensities = spotLight->color * spotLight->intensity;
            light.attenuation = spotLight->attenuation;
            light.ambientCoefficient = spotLight->ambientCoefficient;
            light.coneAngle = spotLight->coneAngle;
            light.direction = glm::vec3(direction);
            light.shadow = spotLight->shadow ? 1.f : 0.f;
            light.distance = spotLight->distance;
            lights.push_back(light);
        }
    }

    // Add all point lights.
    for (Component::PointLight* pointLight : pointLights.GetAll()) {
        if (pointLight->IsKilled() || !pointLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = pointLight->entity;
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->GetWorldPosition()) * glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f) * pointLight->distance);

        Video::Frustum frustum(viewProjectionMatrix * modelMat);
        if (frustum.Collide(aabb)) {
            if (lightVolumes)
                Managers().debugDrawingManager->AddSphere(lightEntity->GetWorldPosition(), pointLight->distance, glm::vec3(1.0f, 1.0f, 1.0f));

            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            Video::Light light;
            light.position = viewMatrix * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0));
            light.intensities = pointLight->color * pointLight->intensity;
            light.attenuation = pointLight->attenuation;
            light.ambientCoefficient = 0.f;
            light.coneAngle = 180.f;
            light.direction = glm::vec3(1.f, 0.f, 0.f);
            light.shadow = 0.f;
            light.distance = pointLight->distance;
            lights.push_back(light);
        }
    }

    lightCount = lights.size();

    // Update light buffer.
    renderer->SetLights(lights);
}

void RenderManager::LightAmbient() {
    std::vector<Video::Light> lights;

    Video::Light light;
    light.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light.intensities = glm::vec3(0.0f, 0.0f, 0.0f);
    light.attenuation = 1.f;
    light.ambientCoefficient = 1.0f;
    light.coneAngle = 0.f;
    light.direction = glm::vec3(0.f, 0.f, 0.f);
    light.shadow = 0.f;
    light.distance = 0.f;
    lights.push_back(light);

    // Update light buffer.
    renderer->SetLights(lights);
}

void RenderManager::LoadTexture(TextureAsset*& texture, const std::string& name) {
    if (!name.empty())
        texture = Managers().resourceManager->CreateTextureAsset(name);
}
