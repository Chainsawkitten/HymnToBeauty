#include "Game.hpp"

#include "Hymn.hpp"
#include <GLFW/glfw3.h>

Game::Game() {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(640, 480, "Hymn to Beauty", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    glfwMakeContextCurrent(window);
}

Game::~Game() {
    glfwDestroyWindow(window);
}

bool Game::ShouldClose() const {
    return (glfwWindowShouldClose(window) != 0);
}

void Game::Update() {

}

void Game::Render() {
    glfwMakeContextCurrent(window);
    
    Hymn().Render(GetSize());
    
    glfwSwapBuffers(window);
}

glm::vec2 Game::GetSize() const {
    return glm::vec2(640.f, 480.f);
}
