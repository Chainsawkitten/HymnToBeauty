#include "RenderManager.hpp"

#include <Video/Renderer.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"
#include "PhysicsManager.hpp"
#include "DebugDrawingManager.hpp"
#include "../Entity/Entity.hpp"
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

void RenderManager::Render(World& world, bool soundSources, bool lightSources, bool cameras, bool physics, Entity* camera, bool lighting, bool lightVolumes) {
    const glm::uvec2 windowSize = MainWindow::GetInstance()->GetSize();
    if (windowSize.x == 0 || windowSize.y == 0)
        return;

    // Find camera entity.
    if (camera == nullptr) {
        for (Lens* lens : lenses.GetAll()) {
            if (lens->entity->IsEnabled())
                camera = lens->entity;
        }
    }

    renderer->BeginFrame();

    if (camera == nullptr) {
        renderer->StartDepthPrePass();
        renderer->StartMainPass();

        Video::PostProcessing::Configuration configuration;
        renderer->ConfigurePostProcessing(configuration);
        renderer->ApplyPostProcessing();

        return;
    }

    // Set image processing variables.
    renderer->SetGamma(Hymn().filterSettings.gamma);

    // Render main window.
    {
        PROFILE("Render main window");
        Lens* lens = camera->GetComponent<Lens>();
        const glm::mat4 projectionMatrix = lens->GetProjection(windowSize);
        const glm::mat4 viewMatrix = glm::inverse(camera->GetModelMatrix());
        const glm::vec3 position = camera->GetWorldPosition();
        const glm::vec3 up(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

        {
            PROFILE("Render world entities");

            RenderWorldEntities(world, viewMatrix, projectionMatrix, lighting, lens->zNear, lens->zFar, lightVolumes);
        }

        {
            PROFILE("Render debug entities");

            Managers().debugDrawingManager->Render(viewMatrix, projectionMatrix);
        }

        {
            PROFILE("Post-processing");

            // Configure.
            Video::PostProcessing::Configuration configuration;
            configuration.gamma = Hymn().filterSettings.gamma;
            configuration.fxaa.enabled = Hymn().filterSettings.fxaa;
            configuration.dither.enabled = Hymn().filterSettings.ditherApply;
            configuration.bloom.enabled = Hymn().filterSettings.bloom;
            configuration.bloom.intensity = Hymn().filterSettings.bloomIntensity;
            configuration.bloom.threshold = Hymn().filterSettings.bloomThreshold;
            configuration.bloom.scatter = Hymn().filterSettings.bloomScatter;
            renderer->ConfigurePostProcessing(configuration);

            // Apply.
            renderer->ApplyPostProcessing();
        }

        if (soundSources || lightSources || cameras || physics) {
            PROFILE("Render editor entities");

            RenderEditorEntities(world, soundSources, lightSources, cameras, physics, position, up, viewMatrix, projectionMatrix);
        }
    }
}

void RenderManager::UpdateBufferSize() {
    renderer->SetRenderSurfaceSize(MainWindow::GetInstance()->GetSize());
}

void RenderManager::RenderWorldEntities(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, bool lighting, float cameraNear, float cameraFar, bool lightVolumes) {
    // Render from camera.
    glm::mat4 lightViewMatrix;
    glm::mat4 lightProjection;

    // Camera matrices.
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    const std::vector<Mesh*>& meshComponents = meshes.GetAll();

    // Get list of entities to render.
    std::vector<Entity*> staticEntities;

    for (Mesh* mesh : meshComponents) {
        Entity* entity = mesh->entity;
        if (entity->IsKilled() || !entity->IsEnabled())
            continue;

        if (mesh->geometry == nullptr || mesh->geometry->GetIndexCount() == 0)
            continue;

        if (entity->GetComponent<Material>() == nullptr)
            continue;

        Video::Frustum frustum(viewProjectionMatrix * entity->GetModelMatrix());
        if (!frustum.Collide(mesh->geometry->GetAxisAlignedBoundingBox()))
            continue;

        staticEntities.push_back(entity);
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

    // Lights.
    {
        PROFILE("Update lights");

        if (lighting)
            // Cull lights and update light list.
            LightWorld(viewMatrix, viewProjectionMatrix, lightVolumes);
        else
            // Use full ambient light and ignore lights in the scene.
            LightAmbient();
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

void RenderManager::RenderEditorEntities(World& world, bool soundSources, bool lightSources, bool cameras, bool physics, const glm::vec3& position, const glm::vec3& up, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

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

void RenderManager::SetDitherApply(bool ditherApply) {
    Hymn().filterSettings.ditherApply = ditherApply;
}

bool RenderManager::GetDitherApply() const {
    return Hymn().filterSettings.ditherApply;
}

unsigned int RenderManager::GetLightCount() const {
    return lightCount;
}

Video::Renderer* RenderManager::GetRenderer() {
    return renderer;
}

void RenderManager::LightWorld(const glm::mat4& viewMatrix, const glm::mat4& viewProjectionMatrix, bool lightVolumes) {
    std::vector<Video::Light> lights;

    Video::AxisAlignedBoundingBox aabb(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    // Add all directional lights.
    for (Component::DirectionalLight* directionalLight : directionalLights.GetAll()) {
        if (directionalLight->IsKilled() || !directionalLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = directionalLight->entity;
        glm::vec4 direction(glm::vec4(lightEntity->GetDirection(), 0.f));
        Video::Light light;
        light.position = viewMatrix * -direction;
        light.intensitiesAttenuation = glm::vec4(directionalLight->color, 1.0f);
        light.directionAmbientCoefficient = glm::vec4(0.0f, 0.0f, 0.0f, directionalLight->ambientCoefficient);
        light.coneAngle = 0.f;
        light.distance = 0.f;
        lights.push_back(light);
    }

    // Add all spot lights.
    for (Component::SpotLight* spotLight : spotLights.GetAll()) {
        if (spotLight->IsKilled() || !spotLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = spotLight->entity;
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->GetWorldPosition()) * glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f) * spotLight->distance);

        // TMPTODO
        Video::Frustum frustum(viewProjectionMatrix * modelMat);
        if (frustum.Collide(aabb)) {
            if (lightVolumes)
                Managers().debugDrawingManager->AddSphere(lightEntity->GetWorldPosition(), spotLight->distance, glm::vec3(1.0f, 1.0f, 1.0f));

            glm::vec4 direction(viewMatrix * glm::vec4(lightEntity->GetDirection(), 0.0f));
            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            Video::Light light;
            light.position = viewMatrix * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0f));
            light.intensitiesAttenuation = glm::vec4(spotLight->color * spotLight->intensity, spotLight->attenuation);
            light.directionAmbientCoefficient = glm::vec4(direction.x, direction.y, direction.z, spotLight->ambientCoefficient);
            light.coneAngle = spotLight->coneAngle;
            light.distance = spotLight->distance;
            lights.push_back(light);
        }
    }

    // Add all point lights.
    for (Component::PointLight* pointLight : pointLights.GetAll()) {
        if (pointLight->IsKilled() || !pointLight->entity->IsEnabled())
            continue;

        Entity* lightEntity = pointLight->entity;
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->GetWorldPosition()) * glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f) * pointLight->distance);

        Video::Frustum frustum(viewProjectionMatrix * modelMat);
        if (frustum.Collide(aabb)) {
            if (lightVolumes)
                Managers().debugDrawingManager->AddSphere(lightEntity->GetWorldPosition(), pointLight->distance, glm::vec3(1.0f, 1.0f, 1.0f));

            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            Video::Light light;
            light.position = viewMatrix * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0f));
            light.intensitiesAttenuation = glm::vec4(pointLight->color * pointLight->intensity, pointLight->attenuation);
            light.directionAmbientCoefficient = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
            light.coneAngle = 180.0f;
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
    light.intensitiesAttenuation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light.directionAmbientCoefficient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light.coneAngle = 0.0f;
    light.distance = 0.0f;
    lights.push_back(light);

    // Update light buffer.
    renderer->SetLights(lights);
}

void RenderManager::LoadTexture(TextureAsset*& texture, const std::string& name) {
    if (!name.empty())
        texture = Managers().resourceManager->CreateTextureAsset(name);
}
