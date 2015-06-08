#include "EditorWindow.hpp"

EditorWindow::EditorWindow() {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

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
