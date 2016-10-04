#include "EntityEditor.hpp"

#include <Engine/Component/Transform.hpp>
#include <Engine/Component/Physics.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Lens.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/PointLight.hpp>
#include <Engine/Component/SpotLight.hpp>
#include <Engine/Component/Listener.hpp>
#include <Engine/Component/SoundSource.hpp>
#include <Engine/Component/ParticleEmitter.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>

using namespace GUI;

EntityEditor::EntityEditor() {
    AddEditor<Component::Transform>("Transform", std::bind(&EntityEditor::TransformEditor, this, std::placeholders::_1));
    AddEditor<Component::Physics>("Physics", std::bind(&EntityEditor::PhysicsEditor, this, std::placeholders::_1));
    AddEditor<Component::Mesh>("Mesh", std::bind(&EntityEditor::MeshEditor, this, std::placeholders::_1));
    AddEditor<Component::Lens>("Lens", std::bind(&EntityEditor::LensEditor, this, std::placeholders::_1));
    AddEditor<Component::Material>("Material", std::bind(&EntityEditor::MaterialEditor, this, std::placeholders::_1));
    AddEditor<Component::DirectionalLight>("Directional light", std::bind(&EntityEditor::DirectionalLightEditor, this, std::placeholders::_1));
    AddEditor<Component::PointLight>("Point light", std::bind(&EntityEditor::PointLightEditor, this, std::placeholders::_1));
    AddEditor<Component::SpotLight>("Spot light", std::bind(&EntityEditor::SpotLightEditor, this, std::placeholders::_1));
    AddEditor<Component::Listener>("Listener", std::bind(&EntityEditor::ListenerEditor, this, std::placeholders::_1));
    AddEditor<Component::SoundSource>("Sound source", std::bind(&EntityEditor::SoundSourceEditor, this, std::placeholders::_1));
    AddEditor<Component::ParticleEmitter>("Particle emitter", std::bind(&EntityEditor::ParticleEmitterEditor, this, std::placeholders::_1));
}

EntityEditor::~EntityEditor() {
    
}

void EntityEditor::Show() {
    if (ImGui::Begin(("Entity: " + entity->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(entity))).c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        entity->name = name;
        
        if (ImGui::Button("Add component"))
            ImGui::OpenPopup("Add component");
        
        if (ImGui::BeginPopup("Add component")) {
            ImGui::Text("Components");
            ImGui::Separator();
            
            for (Editor& editor : editors) {
                editor.addFunction();
            }
            
            ImGui::EndPopup();
        }
        
        for (Editor& editor : editors) {
            editor.editFunction();
        }
    }
    ImGui::End();
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    strcpy(name, entity->name.c_str());
}

bool EntityEditor::IsVisible() const {
    return visible;
}

void EntityEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void EntityEditor::TransformEditor(Component::Transform* transform) {
    ImGui::InputFloat3("Position", &transform->position[0]);
    ImGui::InputFloat3("Rotation", &transform->rotation[0]);
    ImGui::InputFloat3("Scale", &transform->scale[0]);
}

void EntityEditor::PhysicsEditor(Component::Physics* physics) {
    ImGui::InputFloat3("Velocity", &physics->velocity[0]);
    ImGui::InputFloat("Max velocity", &physics->maxVelocity);
    ImGui::InputFloat3("Angular velocity", &physics->angularVelocity[0]);
    ImGui::InputFloat("Max angular velocity", &physics->maxAngularVelocity);
    ImGui::InputFloat3("Acceleration", &physics->acceleration[0]);
    ImGui::InputFloat3("Angular acceleration", &physics->angularAcceleration[0]);
    ImGui::InputFloat("Velocity drag factor", &physics->velocityDragFactor);
    ImGui::InputFloat("Angular drag factor", &physics->angularDragFactor);
    ImGui::InputFloat("Gravity factor", &physics->gravityFactor);
    ImGui::InputFloat3("Moment of inertia", &physics->momentOfInertia[0]);
}

void EntityEditor::MeshEditor(Component::Mesh* mesh) {
    if (ImGui::Button("Select model"))
        ImGui::OpenPopup("Select model");
    
    if (ImGui::BeginPopup("Select model")) {
        ImGui::Text("Models");
        ImGui::Separator();
        
        for (Geometry::Model* model : Hymn().models) {
            if (ImGui::Selectable(model->name.c_str()))
                mesh->geometry = model;
        }
        
        ImGui::EndPopup();
    }
}

void EntityEditor::LensEditor(Component::Lens* lens) {
    ImGui::InputFloat("Field of view", &lens->fieldOfView);
    ImGui::InputFloat("Z near", &lens->zNear);
    ImGui::InputFloat("Z far", &lens->zFar);
}

void EntityEditor::MaterialEditor(Component::Material* material) {
    // Diffuse
    if (ImGui::Button("Select diffuse texture"))
        ImGui::OpenPopup("Select diffuse texture");
    
    if (ImGui::BeginPopup("Select diffuse texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->diffuse = texture;
        }
        
        ImGui::EndPopup();
    }
    
    // Normal
    if (ImGui::Button("Select normal texture"))
        ImGui::OpenPopup("Select normal texture");
    
    if (ImGui::BeginPopup("Select normal texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->normal = texture;
        }
        
        ImGui::EndPopup();
    }
    
    // Specular
    if (ImGui::Button("Select specular texture"))
        ImGui::OpenPopup("Select specular texture");
    
    if (ImGui::BeginPopup("Select specular texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->specular = texture;
        }
        
        ImGui::EndPopup();
    }
    
    // Glow
    if (ImGui::Button("Select glow texture"))
        ImGui::OpenPopup("Select glow texture");
    
    if (ImGui::BeginPopup("Select glow texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->glow = texture;
        }
        
        ImGui::EndPopup();
    }
}

void EntityEditor::DirectionalLightEditor(Component::DirectionalLight* directionalLight) {
    ImGui::InputFloat3("Color", &directionalLight->color[0]);
    ImGui::InputFloat("Ambient coefficient", &directionalLight->ambientCoefficient);
}

void EntityEditor::PointLightEditor(Component::PointLight* pointLight) {
    ImGui::InputFloat3("Color", &pointLight->color[0]);
    ImGui::InputFloat("Ambient coefficient", &pointLight->ambientCoefficient);
    ImGui::InputFloat("Attenuation", &pointLight->attenuation);
    ImGui::InputFloat("Intensity", &pointLight->intensity);
}

void EntityEditor::SpotLightEditor(Component::SpotLight* spotLight) {
    ImGui::InputFloat3("Color", &spotLight->color[0]);
    ImGui::InputFloat("Ambient coefficient", &spotLight->ambientCoefficient);
    ImGui::InputFloat("Attenuation", &spotLight->attenuation);
    ImGui::InputFloat("Intensity", &spotLight->intensity);
    ImGui::InputFloat("Cone angle", &spotLight->coneAngle);
}

void EntityEditor::ListenerEditor(Component::Listener* listener) {
    
}

void EntityEditor::SoundSourceEditor(Component::SoundSource* soundSource) {
    if (ImGui::Button("Select sound"))
        ImGui::OpenPopup("Select sound");
    
    if (ImGui::BeginPopup("Select sound")) {
        ImGui::Text("Sounds");
        ImGui::Separator();
        
        for (Audio::SoundBuffer* sound : Hymn().sounds) {
            if (ImGui::Selectable(sound->name.c_str()))
                soundSource->soundBuffer = sound;
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::InputFloat("Pitch", &soundSource->pitch);
    ImGui::InputFloat("Gain", &soundSource->gain);
    ImGui::Checkbox("Loop", &soundSource->loop);
}

void EntityEditor::ParticleEmitterEditor(Component::ParticleEmitter* particleEmitter) {
    ImGui::InputInt("Texture index", &particleEmitter->particleType.textureIndex);
    ImGui::InputFloat3("Min velocity", &particleEmitter->particleType.minVelocity[0]);
    ImGui::InputFloat3("Max velocity", &particleEmitter->particleType.maxVelocity[0]);
    ImGui::InputFloat("Min lifetime", &particleEmitter->particleType.minLifetime);
    ImGui::InputFloat("Max lifetime", &particleEmitter->particleType.maxLifetime);
    ImGui::InputFloat2("Min size", &particleEmitter->particleType.minSize[0]);
    ImGui::InputFloat2("Max size", &particleEmitter->particleType.maxSize[0]);
    ImGui::Checkbox("Uniform scaling", &particleEmitter->particleType.uniformScaling);
    ImGui::InputFloat("Start alpha", &particleEmitter->particleType.startAlpha);
    ImGui::InputFloat("Mid alpha", &particleEmitter->particleType.midAlpha);
    ImGui::InputFloat("End alpha", &particleEmitter->particleType.endAlpha);
    ImGui::InputFloat3("Color", &particleEmitter->particleType.color[0]);
    ImGui::InputFloat3("Size", &particleEmitter->size[0]);
    ImGui::InputFloat("Min emit time", &particleEmitter->minEmitTime);
    ImGui::InputFloat("Max emit time", &particleEmitter->maxEmitTime);
    
    if (ImGui::Button("Emitter type"))
        ImGui::OpenPopup("Emitter type");
    
    if (ImGui::BeginPopup("Emitter type")) {
        ImGui::Text("Emitter type");
        ImGui::Separator();
        
        if (ImGui::Selectable("Point"))
            particleEmitter->emitterType = Component::ParticleEmitter::POINT;
        
        if (ImGui::Selectable("Cuboid"))
            particleEmitter->emitterType = Component::ParticleEmitter::CUBOID;
        
        ImGui::EndPopup();
    }
}
