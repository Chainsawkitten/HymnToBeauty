#include "ResourceList.hpp"

#include <Engine/Geometry/RiggedModel.hpp>
#include <Engine/Geometry/StaticModel.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>

#include <Engine/Hymn.hpp>
#include <Engine/Scene/Scene.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/MainWindow.hpp>
#include <imgui.h>

using namespace GUI;

void ResourceList::Show() {
    ImGui::Begin("Resources");
    
    // Entities.
    Entity* selected_entity = nullptr;
    if (ImGui::TreeNode("Entities")) {
        if (ImGui::Button("Add entity"))
            Hymn().activeScene.CreateEntity("Entity #" + std::to_string(Hymn().entityNumber++));
        
        for (Entity* entity : Hymn().activeScene.GetEntities()) {
            if (ImGui::Selectable(entity->name.c_str())) {
                selected_entity = entity;
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
        
    // Models.
    Geometry::Model* selected_model = nullptr;
    if (ImGui::TreeNode("Models")) {
        if (ImGui::Button("Add rigged model")) {
            Geometry::Model* model = new Geometry::RiggedModel();
            model->name = "RiggedModel #" + std::to_string(Hymn().modelNumber++);
            Hymn().models.push_back(model);
        }
        if (ImGui::Button("Add static model")) {
            Geometry::Model* model = new Geometry::StaticModel();
            model->name = "StaticModel #" + std::to_string(Hymn().modelNumber++);
            Hymn().models.push_back(model);
        }
        
        for (auto it = Hymn().models.begin(); it != Hymn().models.end(); ++it) {
            Geometry::Model* model = *it;
            if (ImGui::Selectable(model->name.c_str())) {
                selected_model = model;
                modelEditors[model].SetVisible(true);
                modelEditors[model].SetModel(model);
            }
            
            if (ImGui::BeginPopupContextItem(model->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    delete model;
                    Hymn().models.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
        
    // Textures.
    Texture2D* selected_texture = nullptr;
    if (ImGui::TreeNode("Textures")) {
        if (ImGui::Button("Add texture")) {
            Texture2D* texture = new Texture2D();
            texture->name = "Texture #" + std::to_string(Hymn().textureNumber++);
            Hymn().textures.push_back(texture);
        }
        
        for (auto it = Hymn().textures.begin(); it != Hymn().textures.end(); ++it) {
            Texture2D* texture = *it;
            if (ImGui::Selectable(texture->name.c_str())) {
                selected_texture = texture;
                textureEditors[texture].SetVisible(true);
                textureEditors[texture].SetTexture(texture);
            }
            
            if (ImGui::BeginPopupContextItem(texture->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    delete texture;
                    Hymn().textures.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    // Sounds.
    Audio::SoundBuffer* selected_sound = nullptr;
    if (ImGui::TreeNode("Sounds")) {
        if (ImGui::Button("Add sound")) {
            Audio::SoundBuffer* sound = new Audio::SoundBuffer();
            sound->name = "Sound #" + std::to_string(Hymn().soundNumber++);
            Hymn().sounds.push_back(sound);
        }
        
        for (auto it = Hymn().sounds.begin(); it != Hymn().sounds.end(); ++it) {
            Audio::SoundBuffer* sound = *it;
            if (ImGui::Selectable(sound->name.c_str())) {
                selected_sound = sound;
                soundEditors[sound].SetVisible(true);
                soundEditors[sound].SetSound(sound);
            }
            
            if (ImGui::BeginPopupContextItem(sound->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    delete sound;
                    Hymn().sounds.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }

    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);
    ImGui::SetNextWindowPos(ImVec2(size.x - 250, 0));
    ImGui::SetNextWindowSize(ImVec2(250, size.y));

    bool no_selection = selected_entity == nullptr && selected_texture == nullptr && selected_sound == nullptr && selected_model == nullptr;

    // Entity editors.
    if (!no_selection) {
        for (Entity* entity : Hymn().activeScene.GetEntities()) {
            if (entityEditors[entity].IsVisible()) {
                if(entity != selected_entity)
                    entityEditors[entity].SetVisible(false);
                else entityEditors[entity].Show();
            } 
        }
    }
    else{
        for (Entity* entity : Hymn().activeScene.GetEntities()) {
            if (entityEditors[entity].IsVisible()) {
                entityEditors[entity].Show();
            }
        }
    }

    // Model editors.
    if (!no_selection) {
        for (Geometry::Model* models : Hymn().models) {
            if (modelEditors[models].IsVisible()) {
                if (models != selected_model)
                    modelEditors[models].SetVisible(false);
                else modelEditors[models].Show();
            }
        }
    }
    else{
        for (Geometry::Model* model : Hymn().models) {
            if (modelEditors[model].IsVisible()) {
                modelEditors[model].Show();
            }
        }
    }

    // Texture editors.
    if (!no_selection) {
        for (Texture2D* texture : Hymn().textures) {
            if (textureEditors[texture].IsVisible()) {
                if (texture != selected_texture)
                    textureEditors[texture].SetVisible(false);
                else textureEditors[texture].Show();
            }
        }
    }
    else{
        for (Texture2D* texture : Hymn().textures) {
            if (textureEditors[texture].IsVisible()) {
                textureEditors[texture].Show();
            }
        }
    }

    // Sound editors.
    if (!no_selection) {
        for (Audio::SoundBuffer* sound : Hymn().sounds) {
            if (soundEditors[sound].IsVisible()) {
                if (sound != selected_sound)
                    soundEditors[sound].SetVisible(false);
                else soundEditors[sound].Show();
            }
        }
    }
    else{
        for (Audio::SoundBuffer* sound : Hymn().sounds) {
            if (soundEditors[sound].IsVisible()) {
                soundEditors[sound].Show();
            }
        }
    }
    ImGui::End();
}

bool ResourceList::IsVisible() const {
    return visible;
}

void ResourceList::SetVisible(bool visible) {
    this->visible = visible;
}

void ResourceList::HideEditors() {
    for (auto& editor : entityEditors) {
        editor.second.SetVisible(false);
    }
    
    for (auto& editor : modelEditors) {
        editor.second.SetVisible(false);
    }
    
    for (auto& editor : textureEditors) {
        editor.second.SetVisible(false);
    }
    
    for (auto& editor : soundEditors) {
        editor.second.SetVisible(false);
    }
}
