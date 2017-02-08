#include "ResourceList.hpp"

#include <Engine/Geometry/RiggedModel.hpp>
#include <Engine/Geometry/StaticModel.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>

#include <Engine/Hymn.hpp>
#include <imgui.h>

using namespace GUI;

void ResourceList::Show() {
    ImGui::Begin("Resources");
    
    // Scenes.
    if (ImGui::TreeNode("Scenes")) {
        if (ImGui::Button("Add scene"))
            Hymn().scenes.push_back("Scene #" + std::to_string(Hymn().scenes.size()));
        
        for (std::size_t i = 0; i < Hymn().scenes.size(); ++i) {
            if (ImGui::Selectable(Hymn().scenes[i].c_str())) {
                sceneEditor.Save();
                sceneEditor.SetVisible(true);
                sceneEditor.SetScene(&Hymn().scenes[i]);
            }
            
            if (ImGui::BeginPopupContextItem(Hymn().scenes[i].c_str())) {
                if (ImGui::Selectable("Delete")) {
                    Hymn().scenes.erase(Hymn().scenes.begin() + i);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    // Scene editor.
    if (sceneEditor.IsVisible())
        sceneEditor.Show();
    
    // Models.
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
    
    // Model editors.
    for (Geometry::Model* model : Hymn().models) {
        if (modelEditors[model].IsVisible()) {
            modelEditors[model].Show();
        }
    }
    
    // Textures.
    if (ImGui::TreeNode("Textures")) {
        if (ImGui::Button("Add texture")) {
            Texture2D* texture = new Texture2D();
            texture->name = "Texture #" + std::to_string(Hymn().textureNumber++);
            Hymn().textures.push_back(texture);
        }
        
        for (auto it = Hymn().textures.begin(); it != Hymn().textures.end(); ++it) {
            Texture2D* texture = *it;
            if (ImGui::Selectable(texture->name.c_str())) {
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
    
    // Texture editors.
    for (Texture2D* texture : Hymn().textures) {
        if (textureEditors[texture].IsVisible()) {
            textureEditors[texture].Show();
        }
    }
    
    // Sounds.
    if (ImGui::TreeNode("Sounds")) {
        if (ImGui::Button("Add sound")) {
            Audio::SoundBuffer* sound = new Audio::SoundBuffer();
            sound->name = "Sound #" + std::to_string(Hymn().soundNumber++);
            Hymn().sounds.push_back(sound);
        }
        
        for (auto it = Hymn().sounds.begin(); it != Hymn().sounds.end(); ++it) {
            Audio::SoundBuffer* sound = *it;
            if (ImGui::Selectable(sound->name.c_str())) {
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
    
    // Sound editors.
    for (Audio::SoundBuffer* sound : Hymn().sounds) {
        if (soundEditors[sound].IsVisible()) {
            soundEditors[sound].Show();
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
    sceneEditor.SetVisible(false);
    
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

void ResourceList::SaveScene() const {
    sceneEditor.Save();
}
