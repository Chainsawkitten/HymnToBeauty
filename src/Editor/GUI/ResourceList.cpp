#include "ResourceList.hpp"

#include <Engine/Geometry/RiggedModel.hpp>
#include <Engine/Geometry/StaticModel.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Editor/Util/EditorSettings.hpp>

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
    for (Entity* entity : Hymn().activeScene.GetEntities()) {
        if (entityEditors[entity].IsVisible()) {
            entityEditors[entity].Show();
        }
    }
    
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
	// Scripts.
	if (ImGui::TreeNode("Scripts")) {
		if (ImGui::Button("Add script")) {

			std::string name = "Script #" + std::to_string(Hymn().scriptNumber++);
			std::string* filename = new std::string(Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER + name + ".as");
			FileSystem::ExecuteProgram(EditorSettings::GetInstance().GetString("Text Editor"), "\"" + *filename + "\"");
			ScriptFile* script_file = new ScriptFile();
            script_file->name = name;
            script_file->module = name;
			script_file->path = *filename;
			Hymn().scripts.push_back(script_file);

		}

		int script_number = 0;
		for (auto it = Hymn().scripts.begin(); it != Hymn().scripts.end(); ++it) {
			
			ScriptFile* script = *it;
			std::string name = script->name;

			if (ImGui::Selectable(name.c_str())) {
				scriptEditors[script].SetVisible(true);
				scriptEditors[script].SetScript(script);
			}

			if (ImGui::BeginPopupContextItem(name.c_str())) {
				if (ImGui::Selectable("Delete")) {
					ImGui::Text(script->path.c_str());
					Hymn().scripts.erase(it);
					ImGui::EndPopup();
					break;
				}
				ImGui::EndPopup();
			}
		}

		ImGui::TreePop();
	}

	// Script editors.
	for (ScriptFile* script : Hymn().scripts) {
		if (scriptEditors[script].IsVisible()) {
			scriptEditors[script].Show();
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

	for (auto& editor : scriptEditors) {
		editor.second.SetVisible(false);
	}

}
