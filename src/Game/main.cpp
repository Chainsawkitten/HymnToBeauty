#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/GameWindow.hpp>
#include <Engine/Manager/Managers.hpp>

int main() {
    if (!glfwInit())
        return 1;

    GameWindow* gameWindow = new GameWindow();
    glewInit();
    
    Managers().StartUp(gameWindow->GetSize());

    while (!gameWindow->ShouldClose()) {
        gameWindow->Update();
        gameWindow->Render();
        glfwPollEvents();
    }

    delete gameWindow;
    
    Managers().ShutDown();

    glfwTerminate();

    return 0;
}
