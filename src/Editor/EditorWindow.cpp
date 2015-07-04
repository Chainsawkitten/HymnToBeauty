#include <GL/glew.h>
#include "EditorWindow.hpp"

#include "Util/EditorSettings.hpp"
#include <Core/Util/Log.hpp>

EditorWindow::EditorWindow() {
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
    glfwDestroyWindow(window);
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
    }
}

void EditorWindow::Render() {
    if (gameWindow != nullptr)
        gameWindow->Render();

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}
