#include "game.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Core/GameWindow.hpp>

int main() {
    if (!glfwInit())
        return 1;

    GameWindow* gameWindow = new GameWindow();
    glewInit();

    while (!gameWindow->ShouldClose()) {
        gameWindow->Update();
        gameWindow->Render();
        glfwPollEvents();
    }

    delete gameWindow;

    glfwTerminate();

    return 0;
}
