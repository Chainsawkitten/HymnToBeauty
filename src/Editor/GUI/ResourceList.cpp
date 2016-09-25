#include "ResourceList.hpp"

#include <Engine/Geometry/OBJModel.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>

#include <Engine/Hymn.hpp>
#include <Engine/Scene/Scene.hpp>
#include <Engine/Entity/Entity.hpp>
#include <imgui.h>

using namespace GUI;

void ResourceList::Show() {
    ImGui::Begin("Resources");
    
    // Entities.
    if (ImGui::TreeNode("Entities")) {
        if (ImGui::Button("Add entity"))
            Hymn().activeScene.CreateEntity("Entity #" + std::to_string(Hymn().entityNumber++));
        
        for (Entity* entity : Hymn().activeScene.GetEntities()) {
            if (ImGui::Selectable(entity->name.c_str())) {
                entitySelectedCallback(entity);
            }
        }
        
        ImGui::TreePop();
    }
    
    // Models.
    if (ImGui::TreeNode("Models")) {
        if (ImGui::Button("Add model")) {
            Geometry::OBJModel* model = new Geometry::OBJModel();
            model->name = "Model #" + std::to_string(Hymn().modelNumber++);
            Hymn().models.push_back(model);
        }
        
        for (Geometry::OBJModel* model : Hymn().models) {
            if (ImGui::Selectable(model->name.c_str())) {
                modelSelectedCallback(model);
            }
        }
        
        ImGui::TreePop();
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
                textureSelectedCallback(texture);
            }
            
            if (ImGui::BeginPopupContextItem(texture->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    delete texture;
                    Hymn().textures.erase(it--);
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    // Sounds.
    if (ImGui::TreeNode("Sounds")) {
        if (ImGui::Button("Add sound")) {
            Audio::SoundBuffer* sound = new Audio::SoundBuffer();
            sound->name = "Sound #" + std::to_string(Hymn().soundNumber++);
            Hymn().sounds.push_back(sound);
        }
        
        for (Audio::SoundBuffer* sound : Hymn().sounds) {
            if (ImGui::Selectable(sound->name.c_str())) {
                soundSelectedCallback(sound);
            }
        }
        
        ImGui::TreePop();
    }
    
    ImGui::End();
}

bool ResourceList::IsVisible() const {
    return visible;
}

void ResourceList::SetVisible(bool visible) {
    this->visible = visible;
}

void ResourceList::SetEntitySelectedCallback(std::function<void(Entity*)> callback) {
    hasEntitySelectedCallback = true;
    entitySelectedCallback = callback;
}

void ResourceList::SetModelSelectedCallback(std::function<void(Geometry::OBJModel*)> callback) {
    hasModelSelectedCallback = true;
    modelSelectedCallback = callback;
}

void ResourceList::SetTextureSelectedCallback(std::function<void(Texture2D*)> callback) {
    hasTextureSelectedCallback = true;
    textureSelectedCallback = callback;
}

void ResourceList::SetSoundSelectedCallback(std::function<void(Audio::SoundBuffer*)> callback) {
    hasSoundSelectedCallback = true;
    soundSelectedCallback = callback;
}
