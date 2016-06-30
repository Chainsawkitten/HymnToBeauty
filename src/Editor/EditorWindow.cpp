#include <GL/glew.h>
#include "EditorWindow.hpp"

#include <Engine/GameWindow.hpp>
#include <Engine/Font/Font.hpp>
#include "GUI/HorizontalLayout.hpp"
#include "GUI/VerticalLayout.hpp"
#include "GUI/Button.hpp"
#include <Engine/Util/Input.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "GUI/SelectHymnWindow.hpp"

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

EditorWindow::EditorWindow() : Container(nullptr) {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Enable debug context and set message callback.
    if (EditorSettings::GetInstance().GetBool("Debug Context"))
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
    window = glfwCreateWindow(EditorSettings::GetInstance().GetLong("Width"), EditorSettings::GetInstance().GetLong("Height"), "Hymn to Beauty", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    glfwMakeContextCurrent(window);

    gameWindow = nullptr;
    childWindow = nullptr;
    input = new InputHandler(window);
}

EditorWindow::~EditorWindow() {
    delete fileButton;
    delete optionsButton;
    delete playButton;
    delete menuBar;
    
    delete newHymnButton;
    delete openHymnButton;
    delete fileMenu;
    
    Managers().resourceManager->FreeTexture2D(fileTexture);
    Managers().resourceManager->FreeTexture2D(optionsTexture);
    Managers().resourceManager->FreeTexture2D(playTexture);
    
    Managers().resourceManager->FreeTexture2D(newHymnTexture);
    Managers().resourceManager->FreeTexture2D(openHymnTexture);
    
    delete input;
    
    Managers().resourceManager->FreeFont(font);
    
    glfwDestroyWindow(window);
}

void EditorWindow::Init() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    // Menu bar.
    menuBar = new GUI::HorizontalLayout(this);
    menuBar->SetSize(glm::vec2(static_cast<float>(width), 64.f));
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
    
    glEnable(GL_DEPTH_TEST);
}

bool EditorWindow::ShouldClose() const {
    return (glfwWindowShouldClose(window) != 0);
}

void EditorWindow::Update() {
    // Handle running game.
    if (gameWindow != nullptr) {
        gameWindow->Update();
        if (gameWindow->ShouldClose()) {
            delete gameWindow;
            gameWindow = nullptr;
        }
    } else if (childWindow != nullptr) {
        input->Update();
        input->SetActive();
        childWindow->Update();
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        Play();
    } else {
        input->Update();
        input->SetActive();
        UpdateWidgets();
    }
}

void EditorWindow::Render() {
    Render(GetSize());
}

void EditorWindow::Render(const glm::vec2& screenSize) {
    if (gameWindow != nullptr) {
        gameWindow->Render();
    } else {
        glfwMakeContextCurrent(window);
        
        Hymn().Render(screenSize);
        
        RenderWidgets(screenSize);
        
        if (childWindow != nullptr)
            childWindow->Render(screenSize);
        
        glfwSwapBuffers(window);
    }
}

glm::vec2 EditorWindow::GetSize() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    return glm::vec2(static_cast<float>(width), static_cast<float>(height));
}

void EditorWindow::SetSize(const glm::vec2& size) {
    /// @todo Resize window.
}

void EditorWindow::OpenFileMenu() {
    fileMenu->SetVisible(!fileMenu->IsVisible());
}

void EditorWindow::OpenProjectOptions() {
    ///@todo Project options
    Log() << "Click test!\n";
}

void EditorWindow::Play() {
    gameWindow = new GameWindow();
}

void EditorWindow::NewHymn() {
    childWindow = new GUI::SelectHymnWindow(this);
    childWindow->SetPosition(glm::vec2(0.f, 0.f));
    childWindow->SetSize(GetSize());
    childWindow->SetClosedCallback(std::bind(&NewHymnClosed, this, std::placeholders::_1));
}

void EditorWindow::NewHymnClosed(const std::string& hymn) {
    // Create new hymn
    if (!hymn.empty()) {
        Hymn().Clear();
        Hymn().SetPath(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
    }
    
    delete childWindow;
    childWindow = nullptr;
    
    fileMenu->SetVisible(false);
}

void EditorWindow::OpenHymn() {
    childWindow = new GUI::SelectHymnWindow(this);
    childWindow->SetPosition(glm::vec2(0.f, 0.f));
    childWindow->SetSize(GetSize());
    childWindow->SetClosedCallback(std::bind(&OpenHymnClosed, this, std::placeholders::_1));
}

void EditorWindow::OpenHymnClosed(const std::string& hymn) {
    // Open hymn.
    if (!hymn.empty())
        Hymn().Load(FileSystem::DataPath("Hymn to Beauty", hymn.c_str()));
    
    delete childWindow;
    childWindow = nullptr;
    
    fileMenu->SetVisible(false);
}
