#include <GL/glew.h>
#include "EditorWindow.hpp"

#include "Util/EditorSettings.hpp"
#include <Core/Util/Log.hpp>

#include <Core/Resources.hpp>
#include <File.png.hpp>
#include <Options.png.hpp>
#include <Play.png.hpp>

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
    
    Resources().FreeTexture2D(fileTexture);
    Resources().FreeTexture2D(optionsTexture);
    Resources().FreeTexture2D(playTexture);
    
    delete input;
    
    glfwDestroyWindow(window);
}

void EditorWindow::Init() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Menu bar.
    menuBar = new GUI::HorizontalLayout(this);
    menuBar->SetSize(glm::vec2(static_cast<float>(width), 64.f));
    AddWidget(menuBar);
    
    fileTexture = Resources().CreateTexture2D(FILE_PNG, FILE_PNG_LENGTH);
    fileButton = new GUI::Button(menuBar, fileTexture);
    GUI::ClickedMethod method = static_cast<GUI::ClickedMethod>(&ClickTest);
    fileButton->SetClickedCallback(method);
    menuBar->AddWidget(fileButton);
    
    optionsTexture = Resources().CreateTexture2D(OPTIONS_PNG, OPTIONS_PNG_LENGTH);
    optionsButton = new GUI::Button(menuBar, optionsTexture);
    optionsButton->SetClickedCallback(method);
    menuBar->AddWidget(optionsButton);
    
    playTexture = Resources().CreateTexture2D(PLAY_PNG, PLAY_PNG_LENGTH);
    playButton = new GUI::Button(menuBar, playTexture);
    playButton->SetClickedCallback(method);
    menuBar->AddWidget(playButton);
    
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
        gameWindow = new GameWindow();
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
    if (gameWindow != nullptr)
        gameWindow->Render();

    glfwMakeContextCurrent(window);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    RenderWidgets(width, height);
    
    glfwSwapBuffers(window);
}

glm::vec2 EditorWindow::Size() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    return glm::vec2(static_cast<float>(width), static_cast<float>(height));
}

void EditorWindow::ClickTest() {
    Log() << "Click test!\n";
}
