#include "SceneEditor.hpp"

#include <Engine/Hymn.hpp>
#include <imgui.h>

using namespace GUI;

void SceneEditor::Show() {
    if (ImGui::Begin(("Scene: " + *scene + "###Scene").c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        *scene = name;
        
        // Entities.
        if (ImGui::TreeNode("Entities")) {
            if (ImGui::Button("Add entity"))
                Hymn().world.CreateEntity("Entity #" + std::to_string(Hymn().entityNumber++));
            
            for (Entity* entity : Hymn().world.GetEntities()) {
                if (ImGui::Selectable(entity->name.c_str())) {
                    entityEditors[entity].SetVisible(true);
                    entityEditors[entity].SetEntity(entity);
                }
                
                if (ImGui::BeginPopupContextItem(entity->name.c_str())) {
                    if (ImGui::Selectable("Delete")) {
                        entity->Kill();
                    }
                    ImGui::EndPopup();
                }
            }
            
            ImGui::TreePop();
        }
        
        // Entity editors.
        for (Entity* entity : Hymn().world.GetEntities()) {
            if (entityEditors[entity].IsVisible()) {
                entityEditors[entity].Show();
            }
        }
    }
    ImGui::End();
}

void SceneEditor::SetScene(std::string* scene) {
    this->scene = scene;
    
    strcpy(name, scene->c_str());
}

bool SceneEditor::IsVisible() const {
    return visible;
}

void SceneEditor::SetVisible(bool visible) {
    this->visible = visible;
}
