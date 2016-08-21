#include "Editor.hpp"

#include <Engine/MainWindow.hpp>
#include <Engine/Font/Font.hpp>
#include "GUI/HorizontalLayout.hpp"
#include "GUI/VerticalLayout.hpp"
#include "GUI/Button.hpp"
#include <Engine/Util/Input.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "GUI/SelectHymnWindow.hpp"
#include "GUI/ResourceList.hpp"
#include "GUI/Editors/EntityEditor.hpp"
#include "GUI/Editors/ModelEditor.hpp"
#include "GUI/Editors/TextureEditor.hpp"
#include "GUI/Editors/SoundEditor.hpp"
#include "GUI/ModelSelector.hpp"
#include "GUI/TextureSelector.hpp"
#include "GUI/FileSelector.hpp"
#include "GUI/Editors/ComponentEditor/ComponentAdder.hpp"

#include "Util/EditorSettings.hpp"
#include <Engine/Util/Log.hpp>

#include "GUI/ImageButton.hpp"
#include "GUI/ImageTextButton.hpp"
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <File.png.hpp>
#include <Options.png.hpp>
#include <Play.png.hpp>
#include <NewHymn.png.hpp>
#include <OpenHymn.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>

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
    
    // Menu bar.
    menuBar = new GUI::HorizontalLayout(this);
    menuBar->SetSize(glm::vec2(GetSize().x, 64.f));
    AddWidget(menuBar);
    
    fileTexture = Managers().resourceManager->CreateTexture2D(FILE_PNG, FILE_PNG_LENGTH);
    fileButton = new GUI::ImageButton(menuBar, fileTexture);
    fileButton->SetClickedCallback(std::bind(&OpenFileMenu, this));
    menuBar->AddWidget(fileButton);
    
    optionsTexture = Managers().resourceManager->CreateTexture2D(OPTIONS_PNG, OPTIONS_PNG_LENGTH);
    optionsButton = new GUI::ImageButton(menuBar, optionsTexture);
    optionsButton->SetClickedCallback(std::bind(&OpenProjectOptions, this));
    menuBar->AddWidget(optionsButton);
    
    playTexture = Managers().resourceManager->CreateTexture2D(PLAY_PNG, PLAY_PNG_LENGTH);
    playButton = new GUI::ImageButton(menuBar, playTexture);
    playButton->SetClickedCallback(std::bind(&Play, this));
    menuBar->AddWidget(playButton);
    
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
    
    // Component adder.
    componentAdder = new GUI::ComponentAdder(this);
    componentAdder->SetSize(GetSize());
    
    // Editors.
    entityEditor = new GUI::EntityEditor(this, modelSelector, textureSelector, componentAdder);
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
    
    // File menu.
    fileMenu = new GUI::VerticalLayout(this);
    fileMenu->SetSize(glm::vec2(256.f, 2.f * 64.f));
    fileMenu->SetPosition(glm::vec2(0.f, 64.f));
    fileMenu->SetVisible(false);
    AddWidget(fileMenu);
    
    newHymnTexture = Managers().resourceManager->CreateTexture2D(NEWHYMN_PNG, NEWHYMN_PNG_LENGTH);
    newHymnButton = new GUI::ImageTextButton(fileMenu, newHymnTexture, font, "New Hymn");
    newHymnButton->SetSize(glm::vec2(256.f, 64.f));
    newHymnButton->SetClickedCallback(std::bind(&NewHymn, this));
    fileMenu->AddWidget(newHymnButton);
    
    openHymnTexture = Managers().resourceManager->CreateTexture2D(OPENHYMN_PNG, OPENHYMN_PNG_LENGTH);
    openHymnButton = new GUI::ImageTextButton(fileMenu, openHymnTexture, font, "Open Hymn");
    openHymnButton->SetSize(glm::vec2(256.f, 64.f));
    openHymnButton->SetClickedCallback(std::bind(&OpenHymn, this));
    fileMenu->AddWidget(openHymnButton);
}

Editor::~Editor() {
    delete fileButton;
    delete optionsButton;
    delete playButton;
    delete menuBar;
    
    delete newHymnButton;
    delete openHymnButton;
    delete fileMenu;
    
    delete resourceList;
    delete entityEditor;
    delete modelEditor;
    delete textureEditor;
    delete soundEditor;
    
    delete fileSelector;
    delete modelSelector;
    delete textureSelector;
    delete componentAdder;
    
    Managers().resourceManager->FreeTexture2D(fileTexture);
    Managers().resourceManager->FreeTexture2D(optionsTexture);
    Managers().resourceManager->FreeTexture2D(playTexture);
    
    Managers().resourceManager->FreeTexture2D(newHymnTexture);
    Managers().resourceManager->FreeTexture2D(openHymnTexture);
    
    Managers().resourceManager->FreeFont(font);
}

void Editor::Update() {
    if (childWindow != nullptr) {
        childWindow->Update();
    } else if (fileSelector->IsVisible()) {
        fileSelector->Update();
    } else if (modelSelector->IsVisible()) {
        modelSelector->Update();
    } else if (textureSelector->IsVisible()) {
        textureSelector->Update();
    } else if (componentAdder->IsVisible()) {
        componentAdder->Update();
    } else {
        UpdateWidgets();
        Hymn().activeScene.ClearKilled();
    }
    
    if (Input()->Triggered(InputHandler::PLAYTEST))
        Play();
}

void Editor::Render() {
    Hymn().Render();
    
    RenderWidgets();
    
    if (childWindow != nullptr)
        childWindow->Render();
    
    if (fileSelector->IsVisible())
        fileSelector->Render();
    
    if (modelSelector->IsVisible())
        modelSelector->Render();
    
    if (textureSelector->IsVisible())
        textureSelector->Render();
    
    if (componentAdder->IsVisible())
        componentAdder->Render();
    
    MainWindow::GetInstance()->SwapBuffers();
}

glm::vec2 Editor::GetSize() const {
    return MainWindow::GetInstance()->GetSize();
}

void Editor::SetSize(const glm::vec2& size) {
    /// @todo Resize window.
}

void Editor::Save() const {
    Hymn().Save();
}

void Editor::OpenFileMenu() {
    fileMenu->SetVisible(!fileMenu->IsVisible());
}

void Editor::OpenProjectOptions() {
    /// @todo Project options
    Log() << "Click test!\n";
}

void Editor::Play() {
    Save();
    SetVisible(false);
}

void Editor::NewHymn() {
    childWindow = new GUI::SelectHymnWindow(this);
    childWindow->SetPosition(glm::vec2(0.f, 0.f));
    childWindow->SetSize(GetSize());
    childWindow->SetClosedCallback(std::bind(&NewHymnClosed, this, std::placeholders::_1));
}

void Editor::NewHymnClosed(const std::string& hymn) {
    // Create new hymn
    if (!hymn.empty()) {
        Hymn().Clear();
        Hymn().SetPath(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList->SetVisible(true);
    }
    
    delete childWindow;
    childWindow = nullptr;
    
    fileMenu->SetVisible(false);
}

void Editor::OpenHymn() {
    childWindow = new GUI::SelectHymnWindow(this);
    childWindow->SetPosition(glm::vec2(0.f, 0.f));
    childWindow->SetSize(GetSize());
    childWindow->SetClosedCallback(std::bind(&OpenHymnClosed, this, std::placeholders::_1));
}

void Editor::OpenHymnClosed(const std::string& hymn) {
    // Open hymn.
    if (!hymn.empty()) {
        Hymn().Load(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
        resourceList->SetVisible(true);
    }
    
    delete childWindow;
    childWindow = nullptr;
    
    fileMenu->SetVisible(false);
}

void Editor::EntitySelected(Entity* entity) {
    entityEditor->SetEntity(entity);
    
    entityEditor->SetVisible(true);
    modelEditor->SetVisible(false);
    textureEditor->SetVisible(false);
    soundEditor->SetVisible(false);
}

void Editor::ModelSelected(Geometry::OBJModel* model) {
    modelEditor->SetModel(model);
    
    entityEditor->SetVisible(false);
    modelEditor->SetVisible(true);
    textureEditor->SetVisible(false);
    soundEditor->SetVisible(false);
}

void Editor::TextureSelected(Texture2D* texture) {
    textureEditor->SetTexture(texture);
    
    entityEditor->SetVisible(false);
    modelEditor->SetVisible(false);
    textureEditor->SetVisible(true);
    soundEditor->SetVisible(false);
}

void Editor::SoundSelected(Audio::SoundBuffer* sound) {
    soundEditor->SetSound(sound);
    
    entityEditor->SetVisible(false);
    modelEditor->SetVisible(false);
    textureEditor->SetVisible(false);
    soundEditor->SetVisible(true);
}
