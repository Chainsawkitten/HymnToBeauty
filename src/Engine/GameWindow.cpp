#include "GameWindow.hpp"

#include "Hymn.hpp"

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
    
    Hymn().Render(GetSize());
    
    glfwSwapBuffers(window);
}

glm::vec2 GameWindow::GetSize() const {
    return glm::vec2(640.f, 480.f);
}
