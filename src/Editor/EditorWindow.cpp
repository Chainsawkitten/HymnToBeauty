#include <GL/glew.h>
#include "EditorWindow.hpp"

#include "Util/EditorSettings.hpp"
#include <Core/Util/Log.hpp>

#include "GUI/ImageButton.hpp"
#include "GUI/ImageTextButton.hpp"
#include <Core/Resources.hpp>
#include <File.png.hpp>
#include <Options.png.hpp>
#include <Play.png.hpp>
#include <ABeeZee.ttf.hpp>

EditorWindow::EditorWindow() : Container(nullptr) {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Enable debug context and set message callback.
    if (EditorSettings::GetInstance().GetBool("Debug Context"))
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
    window = glfwCreateWindow(640, 480, "Hymn to Beauty", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    glfwMakeContextCurrent(window);

    gameWindow = nullptr;
    input = new InputHandler(window);
}

EditorWindow::~EditorWindow() {
    delete fileButton;
    delete optionsButton;
    delete playButton;
    delete menuBar;
    
    delete newHymnButton;
    delete openHymnButton;
    delete saveHymnButton;
    delete fileMenu;
    
    Resources().FreeTexture2D(fileTexture);
    Resources().FreeTexture2D(optionsTexture);
    Resources().FreeTexture2D(playTexture);
    
    delete input;
    
    Resources().FreeFont(font);
    
    glfwDestroyWindow(window);
}

void EditorWindow::Init() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    font = Resources().CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    // Menu bar.
    menuBar = new GUI::HorizontalLayout(this);
    menuBar->SetSize(glm::vec2(static_cast<float>(width), 64.f));
    AddWidget(menuBar);
    
    fileTexture = Resources().CreateTexture2D(FILE_PNG, FILE_PNG_LENGTH);
    fileButton = new GUI::ImageButton(menuBar, fileTexture);
    fileButton->SetClickedCallback(std::bind(&OpenFileMenu, this));
    menuBar->AddWidget(fileButton);
    
    optionsTexture = Resources().CreateTexture2D(OPTIONS_PNG, OPTIONS_PNG_LENGTH);
    optionsButton = new GUI::ImageButton(menuBar, optionsTexture);
    optionsButton->SetClickedCallback(std::bind(&OpenProjectOptions, this));
    menuBar->AddWidget(optionsButton);
    
    playTexture = Resources().CreateTexture2D(PLAY_PNG, PLAY_PNG_LENGTH);
    playButton = new GUI::ImageButton(menuBar, playTexture);
    playButton->SetClickedCallback(std::bind(&Play, this));
    menuBar->AddWidget(playButton);
    
    // File menu.
    fileMenu = new GUI::VerticalLayout(this);
    fileMenu->SetSize(glm::vec2(256.f, 3.f * 64.f));
    fileMenu->SetPosition(glm::vec2(0.f, 64.f));
    fileMenu->SetVisible(false);
    AddWidget(fileMenu);
    
    newHymnButton = new GUI::ImageTextButton(fileMenu, fileTexture, font, "New Hymn");
    newHymnButton->SetSize(glm::vec2(256.f, 64.f));
    fileMenu->AddWidget(newHymnButton);
    
    openHymnButton = new GUI::ImageTextButton(fileMenu, fileTexture, font, "Open Hymn");
    openHymnButton->SetSize(glm::vec2(256.f, 64.f));
    fileMenu->AddWidget(openHymnButton);
    
    saveHymnButton = new GUI::ImageTextButton(fileMenu, fileTexture, font, "Save Hymn");
    saveHymnButton->SetSize(glm::vec2(256.f, 64.f));
    fileMenu->AddWidget(saveHymnButton);
    
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
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        Play();
    } else {
        input->Update();
        input->SetActive();
        UpdateWidgets();
    }
}

void EditorWindow::Render() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    Render(width, height);
}

void EditorWindow::Render(int width, int height) {
    if (gameWindow != nullptr) {
        gameWindow->Render();
    } else {
        glfwMakeContextCurrent(window);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        RenderWidgets(width, height);
        
        glfwSwapBuffers(window);
    }
}

glm::vec2 EditorWindow::Size() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    return glm::vec2(static_cast<float>(width), static_cast<float>(height));
}

void EditorWindow::OpenFileMenu() {
    fileMenu->SetVisible(!fileMenu->Visible());
}

void EditorWindow::OpenProjectOptions() {
    ///@todo: Project options
    Log() << "Click test!\n";
}

void EditorWindow::Play() {
    gameWindow = new GameWindow();
}
