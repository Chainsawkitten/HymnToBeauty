#include "SceneEditor.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Util/Log.hpp>
#include <imgui.h>

using namespace GUI;

void SceneEditor::Show() {
    if (ImGui::Begin(("Scene: " + *scene + "###Scene").c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        *scene = name;
        
        // Entities.
        ShowEntity(Hymn().world.GetRoot());
        
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
    
    if (scene != nullptr)
        strcpy(name, scene->c_str());
}

bool SceneEditor::IsVisible() const {
    return visible;
}

void SceneEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void SceneEditor::Save() const {
    if (scene != nullptr) {
        Hymn().world.Save(Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + *scene + ".json");
    }
}

void SceneEditor::ShowEntity(Entity* entity) {
    if (ImGui::TreeNode(entity->name.c_str())) {
        if (ImGui::Button("Edit")) {
            entityEditors[entity].SetVisible(true);
            entityEditors[entity].SetEntity(entity);
        }
        
        if (ImGui::Button("Add child"))
            entity->AddChild("Entity #" + std::to_string(Hymn().entityNumber++));
        
        if (ImGui::Button("Delete"))
            entity->Kill();
        
        for (Entity* child : entity->GetChildren()) {
            ShowEntity(child);
        }
        
        ImGui::TreePop();
    }
}
