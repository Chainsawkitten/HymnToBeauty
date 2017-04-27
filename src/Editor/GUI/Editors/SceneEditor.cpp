#include "SceneEditor.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Util/Log.hpp>
#include <imgui.h>

using namespace GUI;

void SceneEditor::Show() {
    if (ImGui::Begin(("Scene: " + Hymn().scenes[sceneIndex] + "###Scene").c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        Hymn().scenes[sceneIndex] = name;
        
        // Entities.
        entityPressed = false;
        ShowEntity(Hymn().world.GetRoot());
    }
    ImGui::End();
}

void SceneEditor::SetScene(std::size_t sceneIndex) {
    entityEditor.SetVisible(false);
    
    if (sceneIndex < Hymn().scenes.size()) {
        this->sceneIndex = sceneIndex;
        strcpy(name, Hymn().scenes[sceneIndex].c_str());
    }
}

bool SceneEditor::IsVisible() const {
    return visible;
}

void SceneEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void SceneEditor::Save() const {
    if (sceneIndex < Hymn().scenes.size())
        Hymn().world.Save(Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + Hymn().scenes[sceneIndex] + ".json");
}

void SceneEditor::ShowEntity(Entity* entity) {
    bool opened = ImGui::TreeNode(entity->name.c_str());
    
    if (ImGui::BeginPopupContextItem(entity->name.c_str())) {
        if (ImGui::Selectable("Edit")) {
            entityPressed = true;
            entityEditor.SetEntity(entity);
        }
        
        if (!entity->IsScene()) {
            if (ImGui::Selectable("Add child"))
                entity->AddChild("Entity #" + std::to_string(Hymn().entityNumber++));
            
            if (ImGui::Selectable("Instantiate scene"))
                ImGui::OpenPopup("Select scene");
            
            if (ImGui::BeginPopup("Select scene")) {
                ImGui::Text("Scenes");
                ImGui::Separator();
                
                for (const std::string& scene : Hymn().scenes) {
                    if (ImGui::Selectable(scene.c_str()))
                        entity->InstantiateScene(scene);
                }
                
                ImGui::EndPopup();
            }
            
            if (entity != Hymn().world.GetRoot()) {
                if (ImGui::Selectable("Delete")) {
                    entity->Kill();
                    if (entityEditor.ShowsEntity(entity))
                        entityEditor.SetVisible(false);
                }
            }
        }
        ImGui::EndPopup();
    }
    
    if (opened) {
        if (!entity->IsScene()) {
            for (Entity* child : entity->GetChildren()) {
                ShowEntity(child);
            }
        }
        
        ImGui::TreePop();
    }
}
