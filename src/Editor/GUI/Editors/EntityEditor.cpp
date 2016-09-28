#include "EntityEditor.hpp"

#include <Engine/Component/Transform.hpp>
#include <Engine/Component/Physics.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Geometry/OBJModel.hpp>

using namespace GUI;

EntityEditor::EntityEditor() {
    AddEditor<Component::Transform>("Transform", std::bind(&TransformEditor, this, std::placeholders::_1));
    AddEditor<Component::Physics>("Physics", std::bind(&PhysicsEditor, this, std::placeholders::_1));
    AddEditor<Component::Mesh>("Mesh", std::bind(&MeshEditor, this, std::placeholders::_1));
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
        
        for (Geometry::OBJModel* model : Hymn().models) {
            if (ImGui::Selectable(model->name.c_str()))
                mesh->geometry = model;
        }
        
        ImGui::EndPopup();
    }
}
