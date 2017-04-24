#include "Editor.hpp"

#include <Engine/Util/Input.hpp>
#include "Util/EditorSettings.hpp"
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/MainWindow.hpp>
#include <Engine/Component/Lens.hpp>

#include <imgui.h>
#include <GLFW/glfw3.h>

Editor::Editor() {
    // Assign controls.
    Input()->AssignButton(InputHandler::PROFILE, InputHandler::KEYBOARD, GLFW_KEY_F2);
    Input()->AssignButton(InputHandler::PLAYTEST, InputHandler::KEYBOARD, GLFW_KEY_F5);
    Input()->AssignButton(InputHandler::CONTROL, InputHandler::KEYBOARD, GLFW_KEY_LEFT_CONTROL);
    Input()->AssignButton(InputHandler::NEW, InputHandler::KEYBOARD, GLFW_KEY_N);
    Input()->AssignButton(InputHandler::OPEN, InputHandler::KEYBOARD, GLFW_KEY_O);
    
    // Create editor camera.
    cameraWorld = new World();
    cameraEntity = cameraWorld->CreateEntity("Editor Camera");
    cameraEntity->AddComponent<Component::Lens>();
}

Editor::~Editor() {
    delete cameraWorld;
}

void Editor::Show() {
    bool play = false;
    
    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);
    
    // Main menu bar.
    if (ImGui::BeginMainMenuBar()) {
        
        // File menu.
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Hymn", "CTRL+N"))
                NewHymn();
            
            if (ImGui::MenuItem("Open Hymn", "CTRL+O"))
                OpenHymn();
            
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
            if (ImGui::MenuItem("Play", "F5"))
                play = true;
            
            ImGui::EndMenu();
        }
        
        // Input
        if(Hymn().GetPath() != "") {
            if (ImGui::BeginMenu("Input")) {
                inputWindow.SetVisible(true);
                
                ImGui::EndMenu();
            }
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // Show hymn selection window.
    if (selectHymnWindow.IsVisible()) {
        ImGui::SetNextWindowPosCenter();
        selectHymnWindow.Show();
    }
    
    if (inputWindow.IsVisible())
        inputWindow.Show();
    
    // Show resource list.
    if (resourceList.IsVisible()) {
        ImGui::SetNextWindowPos(ImVec2(0, size.y - 250));
        ImGui::SetNextWindowSize(ImVec2(size.x - 250, 250));
        
        resourceList.Show();
    }
    
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
    resourceList.SaveScene();
    Hymn().Save();
}

bool Editor::IsVisible() const {
    return visible;
}

void Editor::SetVisible(bool visible) {
    this->visible = visible;
}

Entity* Editor::GetCamera() const {
    return cameraEntity;
}

void Editor::Play() {
    Save();
    SetVisible(false);
    resourceList.HideEditors();
    resourceList.ResetScene();
    Managers().scriptManager->RegisterInput();
    Managers().scriptManager->BuildAllScripts();
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
        resourceList.ResetScene();
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
        resourceList.ResetScene();
        Hymn().Load(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList.SetVisible(true);
    }
    
    selectHymnWindow.SetVisible(false);
}
