#include "Editor.hpp"

#include <Engine/Util/Input.hpp>
#include "Util/EditorSettings.hpp"
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <imgui.h>
#include <GLFW/glfw3.h>

Editor::Editor() {
    // Assign controls.
    Input()->AssignButton(InputHandler::PROFILE, InputHandler::KEYBOARD, GLFW_KEY_F2);
    Input()->AssignButton(InputHandler::PLAYTEST, InputHandler::KEYBOARD, GLFW_KEY_F5);
    Input()->AssignButton(InputHandler::CONTROL, InputHandler::KEYBOARD, GLFW_KEY_LEFT_CONTROL);
    Input()->AssignButton(InputHandler::NEW, InputHandler::KEYBOARD, GLFW_KEY_N);
    Input()->AssignButton(InputHandler::OPEN, InputHandler::KEYBOARD, GLFW_KEY_O);
}

void Editor::Show() {
    bool play = false;
    
    // Main menu bar.
    if (ImGui::BeginMainMenuBar()) {
        // File menu.
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Hymn", "CTRL+N")) {
                NewHymn();
            }
            if (ImGui::MenuItem("Open Hymn", "CTRL+O")) {
                OpenHymn();
            }
            ImGui::EndMenu();
        }
        
        // View menu.
        if (ImGui::BeginMenu("View")) {
            static bool soundSources = EditorSettings::GetInstance().GetBool("Sound Source Icons");
            ImGui::MenuItem("Sound Sources", "", &soundSources);
            EditorSettings::GetInstance().SetBool("Sound Source Icons", soundSources);
            
            static bool particleEmitters = EditorSettings::GetInstance().GetBool("Particle Emitter Icons");
            ImGui::MenuItem("Particle Emitters", "", &particleEmitters);
            EditorSettings::GetInstance().SetBool("Particle Emitter Icons", particleEmitters);
            
            static bool lightSources = EditorSettings::GetInstance().GetBool("Light Source Icons");
            ImGui::MenuItem("Light Sources", "", &lightSources);
            EditorSettings::GetInstance().SetBool("Light Source Icons", lightSources);
            
            ImGui::EndMenu();
        }
        
        // Play
        if (ImGui::BeginMenu("Play")) {
            if (ImGui::MenuItem("Play", "F5")) {
				Managers().scriptManager->BuildAllScripts();
                play = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    // Show hymn selection window.
    if (selectHymnWindow.IsVisible())
        selectHymnWindow.Show();
    
    // Show resource list.
    if (resourceList.IsVisible())
        resourceList.Show();
    
    if (Input()->Triggered(InputHandler::PLAYTEST))
        play = true;
    
    if (Input()->Triggered(InputHandler::NEW) && Input()->Pressed(InputHandler::CONTROL))
        NewHymn();
    
    if (Input()->Triggered(InputHandler::OPEN) && Input()->Pressed(InputHandler::CONTROL))
        OpenHymn();
    
    if (play)
        Play();
}

void Editor::Save() const {
    Hymn().Save();
}

bool Editor::IsVisible() const {
    return visible;
}

void Editor::SetVisible(bool visible) {
    this->visible = visible;
}

void Editor::Play() {
    Save();
    SetVisible(false);
    resourceList.HideEditors();
}

void Editor::NewHymn() {
    selectHymnWindow.Scan();
    selectHymnWindow.SetClosedCallback(std::bind(&Editor::NewHymnClosed, this, std::placeholders::_1));
    selectHymnWindow.SetTitle("New Hymn");
    selectHymnWindow.SetOpenButtonName("Create");
    selectHymnWindow.SetVisible(true);
}

void Editor::NewHymnClosed(const std::string& hymn) {
    // Create new hymn
    if (!hymn.empty()) {
        Hymn().Clear();
        Hymn().SetPath(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList.SetVisible(true);
    }
    
    selectHymnWindow.SetVisible(false);
}

void Editor::OpenHymn() {
    selectHymnWindow.Scan();
    selectHymnWindow.SetClosedCallback(std::bind(&Editor::OpenHymnClosed, this, std::placeholders::_1));
    selectHymnWindow.SetTitle("Open Hymn");
    selectHymnWindow.SetOpenButtonName("Open");
    selectHymnWindow.SetVisible(true);
}

void Editor::OpenHymnClosed(const std::string& hymn) {
    // Open hymn.
    if (!hymn.empty()) {
        Hymn().Load(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList.SetVisible(true);
    }
    
    selectHymnWindow.SetVisible(false);
}
