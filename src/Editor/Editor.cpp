#include "Editor.hpp"

#include <Engine/MainWindow.hpp>
#include <Engine/Font/Font.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "GUI/ResourceList.hpp"
#include "GUI/Editors/EntityEditor.hpp"
#include "GUI/Editors/ModelEditor.hpp"
#include "GUI/Editors/TextureEditor.hpp"
#include "GUI/Editors/SoundEditor.hpp"
#include "GUI/ModelSelector.hpp"
#include "GUI/TextureSelector.hpp"
#include "GUI/SoundSelector.hpp"
#include "GUI/FileSelector.hpp"
#include "GUI/Editors/ComponentEditor/ComponentAdder.hpp"

#include "Util/EditorSettings.hpp"
#include <Engine/Util/Log.hpp>

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

Editor::Editor() : Container(nullptr) {
    // Assign controls.
    Input()->AssignButton(InputHandler::CLICK, InputHandler::MOUSE, GLFW_MOUSE_BUTTON_LEFT);
    Input()->AssignButton(InputHandler::BACK, InputHandler::KEYBOARD, GLFW_KEY_BACKSPACE);
    Input()->AssignButton(InputHandler::ERASE, InputHandler::KEYBOARD, GLFW_KEY_DELETE);
    Input()->AssignButton(InputHandler::LEFT, InputHandler::KEYBOARD, GLFW_KEY_LEFT);
    Input()->AssignButton(InputHandler::RIGHT, InputHandler::KEYBOARD, GLFW_KEY_RIGHT);
    Input()->AssignButton(InputHandler::HOME, InputHandler::KEYBOARD, GLFW_KEY_HOME);
    Input()->AssignButton(InputHandler::END, InputHandler::KEYBOARD, GLFW_KEY_END);
    Input()->AssignButton(InputHandler::PLAYTEST, InputHandler::KEYBOARD, GLFW_KEY_F5);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    // Resource list.
    resourceList = new GUI::ResourceList(this);
    resourceList->SetSize(glm::vec2(250.f, GetSize().y - 64.f));
    resourceList->SetPosition(glm::vec2(0.f, 64.f));
    resourceList->SetVisible(false);
    resourceList->SetEntitySelectedCallback(std::bind(&EntitySelected, this, std::placeholders::_1));
    resourceList->SetModelSelectedCallback(std::bind(&ModelSelected, this, std::placeholders::_1));
    resourceList->SetTextureSelectedCallback(std::bind(&TextureSelected, this, std::placeholders::_1));
    resourceList->SetSoundSelectedCallback(std::bind(&SoundSelected, this, std::placeholders::_1));
    AddWidget(resourceList);
    
    // File selector.
    fileSelector = new GUI::FileSelector(this);
    fileSelector->SetSize(GetSize());
    fileSelector->SetExtension("obj");
    
    // Model selector.
    modelSelector = new GUI::ModelSelector(this);
    modelSelector->SetSize(GetSize());
    modelSelector->SetModels(&Hymn().models);
    
    // Texture selector.
    textureSelector = new GUI::TextureSelector(this);
    textureSelector->SetSize(GetSize());
    textureSelector->SetTextures(&Hymn().textures);
    
    // Sound selector.
    soundSelector = new GUI::SoundSelector(this);
    soundSelector->SetSize(GetSize());
    soundSelector->SetSounds(&Hymn().sounds);
    
    // Component adder.
    componentAdder = new GUI::ComponentAdder(this);
    componentAdder->SetSize(GetSize());
    
    // Editors.
    entityEditor = new GUI::EntityEditor(this, modelSelector, textureSelector, soundSelector, componentAdder);
    entityEditor->SetSize(glm::vec2(250.f, GetSize().y - 64.f));
    entityEditor->SetPosition(glm::vec2(GetSize().x - 250.f, 64.f));
    AddWidget(entityEditor);
    
    modelEditor = new GUI::ModelEditor(this, fileSelector);
    modelEditor->SetSize(glm::vec2(250.f, GetSize().y - 64.f));
    modelEditor->SetPosition(glm::vec2(GetSize().x - 250.f, 64.f));
    AddWidget(modelEditor);
    
    textureEditor = new GUI::TextureEditor(this, fileSelector);
    textureEditor->SetSize(glm::vec2(250.f, GetSize().y - 64.f));
    textureEditor->SetPosition(glm::vec2(GetSize().x - 250.f, 64.f));
    AddWidget(textureEditor);
    
    soundEditor = new GUI::SoundEditor(this, fileSelector);
    soundEditor->SetSize(glm::vec2(250.f, GetSize().y - 64.f));
    soundEditor->SetPosition(glm::vec2(GetSize().x - 250.f, 64.f));
    AddWidget(soundEditor);
}

Editor::~Editor() {
    delete resourceList;
    delete entityEditor;
    delete modelEditor;
    delete textureEditor;
    delete soundEditor;
    
    delete fileSelector;
    delete modelSelector;
    delete textureSelector;
    delete soundSelector;
    delete componentAdder;
    
    Managers().resourceManager->FreeFont(font);
}

void Editor::Update() {
    if (fileSelector->IsVisible()) {
        fileSelector->Update();
    } else if (modelSelector->IsVisible()) {
        modelSelector->Update();
    } else if (textureSelector->IsVisible()) {
        textureSelector->Update();
    } else if (soundSelector->IsVisible()) {
        soundSelector->Update();
    } else if (componentAdder->IsVisible()) {
        componentAdder->Update();
    } else {
        UpdateWidgets();
        Hymn().activeScene.ClearKilled();
    }
}

void Editor::Render() {
    Hymn().Render();
    
    RenderWidgets();
    
    if (fileSelector->IsVisible())
        fileSelector->Render();
    
    if (modelSelector->IsVisible())
        modelSelector->Render();
    
    if (textureSelector->IsVisible())
        textureSelector->Render();
    
    if (soundSelector->IsVisible())
        soundSelector->Render();
    
    if (componentAdder->IsVisible())
        componentAdder->Render();
}

glm::vec2 Editor::GetSize() const {
    return MainWindow::GetInstance()->GetSize();
}

void Editor::SetSize(const glm::vec2& size) {
    /// @todo Resize window.
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
        
        // Play
        if (ImGui::BeginMenu("Play")) {
            if (ImGui::MenuItem("Play", "F5")) {
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
    if (resourceList->IsVisible())
        resourceList->Show();
    
    if (Input()->Triggered(InputHandler::PLAYTEST))
        play = true;
    
    if (play)
        Play();
}

void Editor::Save() const {
    Hymn().Save();
}

void Editor::Play() {
    Save();
    SetVisible(false);
    HideEditors();
}

void Editor::NewHymn() {
    selectHymnWindow.Scan();
    selectHymnWindow.SetClosedCallback(std::bind(&NewHymnClosed, this, std::placeholders::_1));
    selectHymnWindow.SetTitle("New Hymn");
    selectHymnWindow.SetOpenButtonName("Create");
    selectHymnWindow.SetVisible(true);
}

void Editor::NewHymnClosed(const std::string& hymn) {
    // Create new hymn
    if (!hymn.empty()) {
        Hymn().Clear();
        Hymn().SetPath(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList->SetVisible(true);
    }
    
    selectHymnWindow.SetVisible(false);
}

void Editor::OpenHymn() {
    selectHymnWindow.Scan();
    selectHymnWindow.SetClosedCallback(std::bind(&OpenHymnClosed, this, std::placeholders::_1));
    selectHymnWindow.SetTitle("Open Hymn");
    selectHymnWindow.SetOpenButtonName("Open");
    selectHymnWindow.SetVisible(true);
}

void Editor::OpenHymnClosed(const std::string& hymn) {
    // Open hymn.
    if (!hymn.empty()) {
        Hymn().Load(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList->SetVisible(true);
    }
    
    selectHymnWindow.SetVisible(false);
}

void Editor::HideEditors() {
    entityEditor->SetVisible(false);
    modelEditor->SetVisible(false);
    textureEditor->SetVisible(false);
    soundEditor->SetVisible(false);
}

void Editor::EntitySelected(Entity* entity) {
    HideEditors();
    entityEditor->SetEntity(entity);
    entityEditor->SetVisible(true);
}

void Editor::ModelSelected(Geometry::OBJModel* model) {
    HideEditors();
    modelEditor->SetModel(model);
    modelEditor->SetVisible(true);
}

void Editor::TextureSelected(Texture2D* texture) {
    HideEditors();
    textureEditor->SetTexture(texture);
    textureEditor->SetVisible(true);
}

void Editor::SoundSelected(Audio::SoundBuffer* sound) {
    HideEditors();
    soundEditor->SetSound(sound);
    soundEditor->SetVisible(true);
}
