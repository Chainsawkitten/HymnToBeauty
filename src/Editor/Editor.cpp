#include "Editor.hpp"

#include "Util/EditorSettings.hpp"
#undef CreateDirectory

#include <Engine/Util/Input.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/MainWindow.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/Lens.hpp>
#include <Engine/Component/Listener.hpp>
#include "ImGui/Theme.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>

Editor::Editor() {
    // Create Hymns directory.
    FileSystem::CreateDirectory((FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns").c_str());
    
    // Load theme.
    std::string theme = EditorSettings::GetInstance().GetString("Theme");
    if (FileSystem::FileExists((FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes" + FileSystem::DELIMITER + theme + ".json").c_str())) {
        ImGui::LoadTheme(theme.c_str());
    } else {
        ImGui::LoadDefaultTheme();
    }
    
    // Assign controls.
    Input()->AssignButton(InputHandler::PROFILE, InputHandler::KEYBOARD, GLFW_KEY_F2);
    Input()->AssignButton(InputHandler::PLAYTEST, InputHandler::KEYBOARD, GLFW_KEY_F5);
    Input()->AssignButton(InputHandler::CONTROL, InputHandler::KEYBOARD, GLFW_KEY_LEFT_CONTROL);
    Input()->AssignButton(InputHandler::NEW, InputHandler::KEYBOARD, GLFW_KEY_N);
    Input()->AssignButton(InputHandler::OPEN, InputHandler::KEYBOARD, GLFW_KEY_O);
    Input()->AssignButton(InputHandler::CAMERA, InputHandler::MOUSE, GLFW_MOUSE_BUTTON_MIDDLE);
    Input()->AssignButton(InputHandler::FORWARD, InputHandler::KEYBOARD, GLFW_KEY_W);
    Input()->AssignButton(InputHandler::BACKWARD, InputHandler::KEYBOARD, GLFW_KEY_S);
    Input()->AssignButton(InputHandler::LEFT, InputHandler::KEYBOARD, GLFW_KEY_A);
    Input()->AssignButton(InputHandler::RIGHT, InputHandler::KEYBOARD, GLFW_KEY_D);
    
    // Create editor camera.
    cameraEntity = cameraWorld.CreateEntity("Editor Camera");
    cameraEntity->AddComponent<Component::Lens>();
    cameraEntity->position.z = 10.0f;
    
    // Create cursors.
    cursors[0] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    cursors[1] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    cursors[2] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    cursors[3] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    cursors[4] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
}

Editor::~Editor() {
    // Destroy cursors.
    for (int i=0; i < 5; ++i) {
        glfwDestroyCursor(cursors[i]);
    }
}

void Editor::Show(float deltaTime) {
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
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Settings"))
                settingsWindow.SetVisible(true);
            
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
            
            static bool cameras = EditorSettings::GetInstance().GetBool("Camera Icons");
            ImGui::MenuItem("Cameras", "", &cameras);
            EditorSettings::GetInstance().SetBool("Camera Icons", cameras);
            
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
    
    // Show settings window.
    if (settingsWindow.IsVisible()) {
        settingsWindow.Show();
    }
    
    // Control the editor camera.
    if (Input()->Pressed(InputHandler::CAMERA)) {
        if (Input()->Triggered(InputHandler::CAMERA)) {
            lastX = Input()->CursorX();
            lastY = Input()->CursorY();
        }
        
        float sensitivity = 0.3f;
        cameraEntity->rotation.x += sensitivity * (Input()->CursorX() - lastX);
        cameraEntity->rotation.y += sensitivity * (Input()->CursorY() - lastY);
        
        lastX = Input()->CursorX();
        lastY = Input()->CursorY();
        
        glm::mat4 orientation = cameraEntity->GetOrientation();
        glm::vec3 backward(orientation[0][2], orientation[1][2], orientation[2][2]);
        glm::vec3 right(orientation[0][0], orientation[1][0], orientation[2][0]);
        float speed = 3.0f * deltaTime;
        cameraEntity->position += speed * backward * static_cast<float>(Input()->Pressed(InputHandler::BACKWARD) - Input()->Pressed(InputHandler::FORWARD));
        cameraEntity->position += speed * right * static_cast<float>(Input()->Pressed(InputHandler::RIGHT) - Input()->Pressed(InputHandler::LEFT));
    }
    
    if (Input()->Triggered(InputHandler::PLAYTEST))
        play = true;
    
    if (Input()->Triggered(InputHandler::NEW) && Input()->Pressed(InputHandler::CONTROL))
        NewHymn();
    
    if (Input()->Triggered(InputHandler::OPEN) && Input()->Pressed(InputHandler::CONTROL))
        OpenHymn();
    
    if (play)
        Play();
    
    // Set cursor.
    if (ImGui::GetMouseCursor() < 5) {
        glfwSetCursor(MainWindow::GetInstance()->GetGLFWWindow(), cursors[ImGui::GetMouseCursor()]);
    }
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
        Hymn().world.CreateRoot();
        Hymn().SetPath(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns" + FileSystem::DELIMITER +  hymn);
        resourceList.SetVisible(true);
        
        // Default scene.
        Hymn().scenes.push_back("Scene #0");
        
        Entity* player = Hymn().world.GetRoot()->AddChild("Player");
        player->position.z = 10.f;
        player->AddComponent<Component::Lens>();
        player->AddComponent<Component::Listener>();
        
        Entity* sun = Hymn().world.GetRoot()->AddChild("Sun");
        sun->AddComponent<Component::DirectionalLight>();
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
        Hymn().Load(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns" + FileSystem::DELIMITER +  hymn);
        resourceList.SetVisible(true);
    }
    
    selectHymnWindow.SetVisible(false);
}
