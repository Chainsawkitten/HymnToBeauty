#include "ResourceList.hpp"

#include <Engine/Geometry/RiggedModel.hpp>
#include <Engine/Geometry/StaticModel.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Editor/Util/EditorSettings.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/MainWindow.hpp>
#include <imgui.h>
#include <limits>
#include "../ImGui/Splitter.hpp"

using namespace GUI;

void ResourceList::Show() {
    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);
    
    // Splitter.
    ImGui::VerticalSplitter(ImVec2(sceneWidth, size.y - resourceHeight), size.x - sceneWidth - editorWidth, splitterSize, resourceHeight, resourceResize, 20, size.y - 20);
    if (resourceResize)
        resourceHeight = size.y - resourceHeight;
    
    ImGui::SetNextWindowPos(ImVec2(sceneWidth, size.y - resourceHeight));
    ImGui::SetNextWindowSize(ImVec2(size.x - sceneWidth - editorWidth, resourceHeight));
    
    ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoResize);
    
    // Scenes.
    if (ImGui::TreeNode("Scenes")) {
        if (ImGui::Button("Add scene"))
            Hymn().scenes.push_back("Scene #" + std::to_string(Hymn().scenes.size()));
        
        for (std::size_t i = 0; i < Hymn().scenes.size(); ++i) {
            if (ImGui::Selectable(Hymn().scenes[i].c_str())) {
                sceneEditor.Save();
                sceneEditor.SetVisible(true);
                sceneEditor.SetScene(i);
                Hymn().activeScene = i;
                sceneEditor.entityEditor.SetVisible(false);
                Hymn().world.Clear();
                std::string sceneFile = Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + Hymn().scenes[i] + ".json";
                Hymn().world.Load(sceneFile);
            }
            
            if (ImGui::BeginPopupContextItem(Hymn().scenes[i].c_str())) {
                if (ImGui::Selectable("Delete")) {
                    Hymn().scenes.erase(Hymn().scenes.begin() + i);
                    ImGui::EndPopup();
                    
                    if (Hymn().activeScene >= i) {
                        if (Hymn().activeScene > 0)
                            Hymn().activeScene = Hymn().activeScene - 1;
                        
                        sceneEditor.SetScene(Hymn().activeScene);
                    }
                    break;
                }
                ImGui::EndPopup();
            }
        }
        ImGui::TreePop();
    }
    
    // Models.
    bool modelPressed = false;
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
                modelPressed = true;
                modelEditor.SetModel(model);
            }
            
            if (ImGui::BeginPopupContextItem(model->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (modelEditor.GetModel() == model)
                        modelEditor.SetVisible(false);
                    
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
    bool texturePressed = false;
    if (ImGui::TreeNode("Textures")) {
        if (ImGui::Button("Add texture")) {
            Texture2D* texture = new Texture2D();
            texture->name = "Texture #" + std::to_string(Hymn().textureNumber++);
            Hymn().textures.push_back(texture);
        }
        
        for (auto it = Hymn().textures.begin(); it != Hymn().textures.end(); ++it) {
            Texture2D* texture = *it;
            if (ImGui::Selectable(texture->name.c_str())) {
                texturePressed = true;
                textureEditor.SetTexture(texture);
            }
            
            if (ImGui::BeginPopupContextItem(texture->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (textureEditor.GetTexture() == texture)
                        textureEditor.SetVisible(false);
                    
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
    
    // Scripts.
    bool scriptPressed = false;
    if (ImGui::TreeNode("Scripts")) {
        if (ImGui::Button("Add script")) {
            ScriptFile* scriptFile = new ScriptFile();
            scriptFile->name = "Script #" + std::to_string(Hymn().scriptNumber++);
            Hymn().scripts.push_back(scriptFile);
        }
        
        for (auto it = Hymn().scripts.begin(); it != Hymn().scripts.end(); ++it) {
            ScriptFile* script = *it;
            std::string name = script->name;
            
            if (ImGui::Selectable(name.c_str())) {
                scriptPressed = true;
                scriptEditor.SetScript(script);
            }
            
            if (ImGui::BeginPopupContextItem(name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (scriptEditor.GetScript() == script)
                        scriptEditor.SetVisible(false);
                    
                    delete script;
                    Hymn().scripts.erase(it);
                    ImGui::EndPopup();
                    break;
                }
                ImGui::EndPopup();
            }
        }
        
        ImGui::TreePop();
    }
    
    // Sounds.
    bool soundPressed = false;
    if (ImGui::TreeNode("Sounds")) {
        if (ImGui::Button("Add sound")) {
            Audio::SoundBuffer* sound = new Audio::SoundBuffer();
            sound->name = "Sound #" + std::to_string(Hymn().soundNumber++);
            Hymn().sounds.push_back(sound);
        }
        
        for (auto it = Hymn().sounds.begin(); it != Hymn().sounds.end(); ++it) {
            Audio::SoundBuffer* sound = *it;
            if (ImGui::Selectable(sound->name.c_str())) {
                soundPressed = true;
                soundEditor.SetSound(sound);
            }
            
            if (ImGui::BeginPopupContextItem(sound->name.c_str())) {
                if (ImGui::Selectable("Delete")) {
                    if (soundEditor.GetSound() == sound)
                        soundEditor.SetVisible(false);
                    
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
    
    if (sceneEditor.entityPressed || scriptPressed || texturePressed || modelPressed || soundPressed) {
        sceneEditor.entityEditor.SetVisible(sceneEditor.entityPressed);
        scriptEditor.SetVisible(scriptPressed);
        textureEditor.SetVisible(texturePressed);
        modelEditor.SetVisible(modelPressed);
        soundEditor.SetVisible(soundPressed);
    }
    
    if (sceneEditor.IsVisible()) {
        ImGui::HorizontalSplitter(ImVec2(sceneWidth, 20), size.y - 20, splitterSize, sceneWidth, sceneResize, 20, size.x - editorWidth - 20);
        ImGui::SetNextWindowPos(ImVec2(0, 20));
        ImGui::SetNextWindowSize(ImVec2(sceneWidth, size.y - 20));
        sceneEditor.Show();
    }
    
    if (sceneEditor.entityEditor.IsVisible() || scriptEditor.IsVisible() || textureEditor.IsVisible() || modelEditor.IsVisible() || soundEditor.IsVisible()) {
        editorWidth = size.x - editorWidth;
        ImGui::HorizontalSplitter(ImVec2(editorWidth, 20), size.y - 20, splitterSize, editorWidth, editorResize, sceneWidth + 20, size.x - 20);
        editorWidth = size.x - editorWidth;
        
        ImGui::SetNextWindowPos(ImVec2(size.x - editorWidth, 20));
        ImGui::SetNextWindowSize(ImVec2(editorWidth, size.y - 20));
    }
    
    if (sceneEditor.entityEditor.IsVisible())
        sceneEditor.entityEditor.Show();
    if (scriptEditor.IsVisible())
        scriptEditor.Show();
    if (textureEditor.IsVisible())
        textureEditor.Show();
    if (modelEditor.IsVisible())
        modelEditor.Show();
    if (soundEditor.IsVisible())
        soundEditor.Show();
    
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
    sceneEditor.entityEditor.SetVisible(false);
    scriptEditor.SetVisible(false);
    modelEditor.SetVisible(false);
    textureEditor.SetVisible(false);
    soundEditor.SetVisible(false);
}

void ResourceList::SaveScene() const {
    sceneEditor.Save();
}

#undef max
void ResourceList::ResetScene() {
    sceneEditor.SetScene(std::numeric_limits<std::size_t>::max());
    sceneEditor.SetVisible(false);
}
