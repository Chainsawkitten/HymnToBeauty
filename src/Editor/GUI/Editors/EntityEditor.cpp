#include "EntityEditor.hpp"

#include <Engine/Entity/Entity.hpp>
#include <imgui.h>
#include <Engine/Component/Transform.hpp>

using namespace GUI;

EntityEditor::EntityEditor() {
    
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
            
            if (entity->GetComponent<Component::Transform>() == nullptr)
                if (ImGui::Selectable("Transform"))
                    entity->AddComponent<Component::Transform>();
            
            ImGui::EndPopup();
        }
        
        Component::Transform* component = entity->GetComponent<Component::Transform>();
        if (component != nullptr && ImGui::CollapsingHeader("Transform")) {
            ImGui::InputFloat3("Position", &component->position[0]);
            ImGui::InputFloat3("Rotation", &component->rotation[0]);
            ImGui::InputFloat3("Scale", &component->scale[0]);
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
