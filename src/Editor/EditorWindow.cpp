#include <GL/glew.h>
#include "EditorWindow.hpp"

#include "Util/EditorSettings.hpp"
#include <Core/Util/Log.hpp>

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
}

EditorWindow::~EditorWindow() {
    delete fileButton;
    delete menuBar;
    
    glfwDestroyWindow(window);
}

void EditorWindow::Init() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Menu bar.
    menuBar = new GUI::HorizontalLayout(this);
    menuBar->SetSize(glm::vec2(static_cast<float>(width), 64.f));
    AddWidget(menuBar);
    
    fileButton = new GUI::Button(menuBar);
    menuBar->AddWidget(fileButton);
}

bool EditorWindow::ShouldClose() const {
    return (glfwWindowShouldClose(window) != 0);
}

void EditorWindow::Update() {
    Update(window);
}

void EditorWindow::Update(GLFWwindow* window) {
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
        UpdateWidgets(window);
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
