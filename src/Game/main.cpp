#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/Game.hpp>
#include <Engine/Manager/Managers.hpp>

int main() {
    if (!glfwInit())
        return 1;

    Game* game = new Game();
    glewInit();
    
    Managers().StartUp(game->GetSize());

    while (!game->ShouldClose()) {
        game->Update();
        game->Render();
        glfwPollEvents();
    }

    delete game;
    
    Managers().ShutDown();

    glfwTerminate();

    return 0;
}
