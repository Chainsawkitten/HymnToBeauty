#include "EntityEditor.hpp"

#include <Engine/Component/Transform.hpp>

using namespace GUI;

EntityEditor::EntityEditor() {
    AddEditor<Component::Transform>("Transform", std::bind(&TransformEditor, this, std::placeholders::_1));
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
