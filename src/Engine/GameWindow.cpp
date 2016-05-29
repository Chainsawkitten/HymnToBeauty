#include "GameWindow.hpp"

GameWindow::GameWindow() {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(640, 480, "Hymn to Beauty", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    glfwMakeContextCurrent(window);
}

GameWindow::~GameWindow() {
    glfwDestroyWindow(window);
}

bool GameWindow::ShouldClose() const {
    return (glfwWindowShouldClose(window) != 0);
}

void GameWindow::Update() {

}

void GameWindow::Render() {
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}