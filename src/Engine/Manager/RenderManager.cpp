#include "RenderManager.hpp"

#include <Video/Renderer.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"
#include "PhysicsManager.hpp"
#include "DebugDrawingManager.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Camera.hpp"
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
#include <Video/Culling/Sphere.hpp>
#include "../MainWindow.hpp"
#include <Video/Lighting/Light.hpp>
#include "../Hymn.hpp"
#include "../Util/Profiling.hpp"
#include "../Util/Json.hpp"
#include <Utility/Log.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Video/Texture/Texture2D.hpp>
#include "Light.png.hpp"
#include "SoundSource.png.hpp"
#include "Camera.png.hpp"

using namespace Component;

RenderManager::RenderManager(Video::Renderer* renderer) {
    assert(renderer != nullptr);
    this->renderer = renderer;

    // Init textures.
    lightTexture = Managers().resourceManager->CreateTexture2D(LIGHT_PNG, LIGHT_PNG_LENGTH);
    soundSourceTexture = Managers().resourceManager->CreateTexture2D(SOUNDSOURCE_PNG, SOUNDSOURCE_PNG_LENGTH);
    cameraTexture = Managers().resourceManager->CreateTexture2D(CAMERA_PNG, CAMERA_PNG_LENGTH);
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeTexture2D(lightTexture);
    Managers().resourceManager->FreeTexture2D(soundSourceTexture);
    Managers().resourceManager->FreeTexture2D(cameraTexture);
}

void RenderManager::Render(World& world, bool showSoundSources, bool showLightSources, bool showCameras, bool showPhysics, Entity* cameraEntity, bool lighting, bool showLightVolumes) {
    const glm::uvec2 windowSize = MainWindow::GetInstance()->GetSize();
    if (windowSize.x == 0 || windowSize.y == 0)
        return;

    // Find camera entity.
    if (cameraEntity == nullptr) {
        for (Camera* camera : cameras.GetAll()) {
            if (camera->entity->IsEnabled())
                cameraEntity = camera->entity;
        }
    }

    renderer->BeginFrame();

    if (cameraEntity == nullptr) {
        renderer->StartDepthPrePass();
        renderer->StartMainPass();

        Video::PostProcessing::Configuration configuration;
        renderer->ConfigurePostProcessing(configuration);
        renderer->ApplyPostProcessing();

        return;
    }

    // Set image processing variables.
    const Camera* camera = cameraEntity->GetComponent<Camera>();
    renderer->SetGamma(camera->filterSettings.gamma);

    // Render main window.
    {
        PROFILE("Render main window");
        const glm::mat4 projectionMatrix = camera->GetProjection(windowSize);
        const glm::mat4 viewMatrix = glm::inverse(cameraEntity->GetModelMatrix());
        const glm::vec3 position = cameraEntity->GetWorldPosition();
        const glm::vec3 up(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

        {
            PROFILE("Render world entities");

            RenderWorldEntities(world, viewMatrix, projectionMatrix, lighting, camera->zNear, camera->zFar, showLightVolumes);
        }

        {
            PROFILE("Render debug entities");

            Managers().debugDrawingManager->Render(viewMatrix, projectionMatrix);
        }

        {
            PROFILE("Post-processing");

            // Configure.
            Video::PostProcessing::Configuration configuration;
            configuration.gamma = camera->filterSettings.gamma;
            configuration.fxaa.enabled = camera->filterSettings.fxaa;
            configuration.dither.enabled = camera->filterSettings.ditherApply;
            configuration.bloom.enabled = camera->filterSettings.bloom;
            configuration.bloom.intensity = camera->filterSettings.bloomIntensity;
            configuration.bloom.threshold = camera->filterSettings.bloomThreshold;
            configuration.bloom.scatter = camera->filterSettings.bloomScatter;
            renderer->ConfigurePostProcessing(configuration);

            // Apply.
            renderer->ApplyPostProcessing();
        }

        if (showSoundSources || showLightSources || showCameras || showPhysics) {
            PROFILE("Render editor entities");

            RenderEditorEntities(world, showSoundSources, showLightSources, showCameras, showPhysics, position, up, viewMatrix, projectionMatrix);
        }
    }
}

void RenderManager::UpdateBufferSize() {
    renderer->SetRenderSurfaceSize(MainWindow::GetInstance()->GetSize());
}

void RenderManager::RenderWorldEntities(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lighting, float cameraNear, float cameraFar, bool showLightVolumes) {
    // Render from camera.
    glm::mat4 lightViewMatrix;
    glm::mat4 lightProjection;

    // Camera matrices.
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    const std::vector<Mesh*>& meshComponents = meshes.GetAll();

    // Lights.
    {
        PROFILE("Update lights");

        if (lighting) {
            // Cull lights and update light list.
            LightWorld(viewMatrix, projectionMatrix, cameraNear, cameraFar, showLightVolumes);
        } else {
            // Use full ambient light and ignore lights in the scene.
            LightAmbient(projectionMatrix, cameraNear, cameraFar);
        }
    }

    // Get list of entities to render.
    std::vector<Entity*> staticEntities;
    {
        PROFILE("Frustum culling");

        for (Mesh* mesh : meshComponents) {
            Entity* entity = mesh->entity;
            if (entity->IsKilled() || !entity->IsEnabled())
                continue;

            if (mesh->geometry == nullptr || mesh->geometry->GetIndexCount() == 0)
                continue;

            if (entity->GetComponent<Material>() == nullptr)
                continue;

            Video::Frustum frustum(viewProjectionMatrix * entity->GetModelMatrix());
            if (!frustum.Intersects(mesh->geometry->GetAxisAlignedBoundingBox()))
                continue;

            staticEntities.push_back(entity);
        }
    }

    // Depth pre-pass.
    renderer->StartDepthPrePass();
    {
        PROFILE("Depth pre-pass");

        // Static meshes.
        if (!staticEntities.empty()) {
            renderer->PrepareStaticMeshDepthRendering(viewMatrix, projectionMatrix);

            for (Entity* entity : staticEntities) {
                renderer->DepthRenderStaticMesh(entity->GetComponent<Mesh>()->geometry, entity->GetModelMatrix());
            }
        }
    }

    // Render meshes.
    renderer->StartMainPass();
    {
        PROFILE("Render meshes");

        // Static meshes.
        if (!staticEntities.empty()) {
            renderer->PrepareStaticMeshRendering(viewMatrix, projectionMatrix, cameraNear, cameraFar);

            for (Entity* entity : staticEntities) {
                Material* material = entity->GetComponent<Material>();

                renderer->RenderStaticMesh(entity->GetComponent<Mesh>()->geometry, material->albedo->GetTexture(), material->normal->GetTexture(), material->roughnessMetallic->GetTexture(), entity->GetModelMatrix());
            }
        }
    }
}

void RenderManager::RenderEditorEntities(World& world, bool showSoundSources, bool showLightSources, bool showCameras, bool showPhysics, const glm::vec3& position, const glm::vec3& up, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

    renderer->PrepareRenderingIcons(viewProjectionMatrix, position, up);

    // Render sound sources.
    if (showSoundSources) {
        for (SoundSource* soundSource : Managers().soundManager->GetSoundSources())
            renderer->RenderIcon(soundSource->entity->GetWorldPosition(), soundSourceTexture);
    }

    // Render light sources.
    if (showLightSources) {
        for (DirectionalLight* light : directionalLights.GetAll())
            renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);

        for (PointLight* light : pointLights.GetAll())
            renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);

        for (SpotLight* light : spotLights.GetAll())
            renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
    }

    // Render cameras.
    if (showCameras) {
        for (Camera* camera : cameras.GetAll())
            renderer->RenderIcon(camera->entity->GetWorldPosition(), cameraTexture);
    }

    // Render physics.
    if (showPhysics) {
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

Component::Camera* RenderManager::CreateCamera() {
    return cameras.Create();
}

Component::Camera* RenderManager::CreateCamera(const Json::Value& node) {
    Component::Camera* camera = cameras.Create();

    // Load values from Json node.
    camera->fieldOfView = node.get("fieldOfView", 45.f).asFloat();
    camera->zNear = node.get("zNear", 0.1f).asFloat();
    camera->zFar = node.get("zFar", 100.f).asFloat();

    const Json::Value& filtersNode = node["filters"];
    camera->filterSettings.gamma = filtersNode.get("gamma", 2.2f).asFloat();
    camera->filterSettings.ditherApply = filtersNode["dither"].asBool();
    camera->filterSettings.fxaa = filtersNode["fxaa"].asBool();
    camera->filterSettings.bloom = filtersNode["bloom"].asBool();
    camera->filterSettings.bloomIntensity = filtersNode.get("bloomIntensity", 1.0f).asFloat();
    camera->filterSettings.bloomThreshold = filtersNode.get("bloomThreshold", 1.0f).asFloat();
    camera->filterSettings.bloomScatter = filtersNode.get("bloomScatter", 0.7f).asFloat();

    return camera;
}

const std::vector<Component::Camera*>& RenderManager::GetCameras() const {
    return cameras.GetAll();
}

Component::Material* RenderManager::CreateMaterial() {
    return materials.Create();
}

Component::Material* RenderManager::CreateMaterial(const Json::Value& node) {
    Component::Material* material = materials.Create();

    // Load values from Json node.
    LoadTexture(material->albedo, node.get("albedo", "").asString());
    LoadTexture(material->normal, node.get("normal", "").asString());
    LoadTexture(material->roughnessMetallic, node.get("roughnessMetallic", "").asString());

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
    spotLight->distance = node.get("distance", 1.f).asFloat();

    return spotLight;
}

const std::vector<Component::SpotLight*>& RenderManager::GetSpotLights() const {
    return spotLights.GetAll();
}

void RenderManager::ClearKilledComponents() {
    directionalLights.ClearKilled();
    cameras.ClearKilled();
    materials.ClearKilled();
    meshes.ClearKilled();
    pointLights.ClearKilled();
    spotLights.ClearKilled();
}

unsigned int RenderManager::GetLightCount() const {
    return lightCount;
}

Video::Renderer* RenderManager::GetRenderer() {
    return renderer;
}

void RenderManager::LightWorld(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float zNear, float zFar, bool showLightVolumes) {
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    const Video::Frustum frustum(viewProjectionMatrix);

    std::vector<Video::DirectionalLight> directionalLightStructs;
    std::vector<Video::Light> lights;

    // Add all directional lights.
    for (Component::DirectionalLight* directionalLight : directionalLights.GetAll()) {
        if (directionalLight->IsKilled() || !directionalLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = directionalLight->entity;
        glm::vec4 direction(glm::vec4(lightEntity->GetDirection(), 0.f));
        Video::DirectionalLight light;
        light.direction = viewMatrix * -direction;
        light.intensitiesAmbientCoefficient = glm::vec4(directionalLight->color, directionalLight->ambientCoefficient);
        directionalLightStructs.push_back(light);
    }

    // Add all spot lights.
    /// @todo More accurate frustum culling.
    for (Component::SpotLight* spotLight : spotLights.GetAll()) {
        if (spotLight->IsKilled() || !spotLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = spotLight->entity;
        glm::vec3 worldPosition = lightEntity->GetWorldPosition();
        Video::Sphere sphere(worldPosition, spotLight->distance);

        if (frustum.Intersects(sphere)) {
            if (showLightVolumes)
                Managers().debugDrawingManager->AddSphere(worldPosition, spotLight->distance, glm::vec3(1.0f, 1.0f, 1.0f));

            glm::vec4 direction(viewMatrix * glm::vec4(lightEntity->GetDirection(), 0.0f));
            glm::vec4 position(viewMatrix * (glm::vec4(worldPosition, 1.0f)));
            Video::Light light;
            light.positionDistance = glm::vec4(position.x, position.y, position.z, spotLight->distance);
            light.intensitiesAttenuation = glm::vec4(spotLight->color * spotLight->intensity, spotLight->attenuation);
            light.directionConeAngle = glm::vec4(direction.x, direction.y, direction.z, spotLight->coneAngle);
            lights.push_back(light);
        }
    }

    // Add all point lights.
    /// @todo More accurate frustum culling.
    for (Component::PointLight* pointLight : pointLights.GetAll()) {
        if (pointLight->IsKilled() || !pointLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = pointLight->entity;
        glm::vec3 worldPosition = lightEntity->GetWorldPosition();
        Video::Sphere sphere(worldPosition, pointLight->distance);

        if (frustum.Intersects(sphere)) {
            if (showLightVolumes)
                Managers().debugDrawingManager->AddSphere(worldPosition, pointLight->distance, glm::vec3(1.0f, 1.0f, 1.0f));

            glm::vec4 position(viewMatrix * (glm::vec4(worldPosition, 1.0f)));
            Video::Light light;
            light.positionDistance = glm::vec4(position.x, position.y, position.z, pointLight->distance);
            light.intensitiesAttenuation = glm::vec4(pointLight->color * pointLight->intensity, pointLight->attenuation);
            light.directionConeAngle = glm::vec4(1.0f, 0.0f, 0.0f, 180.0f);
            lights.push_back(light);
        }
    }

    lightCount = lights.size();

    // Update light buffer.
    renderer->SetLights(directionalLightStructs, lights, projectionMatrix, zNear, zFar);
}

void RenderManager::LightAmbient(const glm::mat4& projectionMatrix, float zNear, float zFar) {
    std::vector<Video::DirectionalLight> directionalLightStructs;
    std::vector<Video::Light> lights;

    Video::DirectionalLight light;
    light.direction = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    light.intensitiesAmbientCoefficient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    directionalLightStructs.push_back(light);

    // Update light buffer.
    renderer->SetLights(directionalLightStructs, lights, projectionMatrix, zNear, zFar);
}

void RenderManager::LoadTexture(TextureAsset*& texture, const std::string& name) {
    if (!name.empty())
        texture = Managers().resourceManager->CreateTextureAsset(name);
}
