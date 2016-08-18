#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/MainWindow.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/Log.hpp>

int main() {
    if (!glfwInit())
        return 1;
    
    Log() << "Game started - " << time(nullptr) << "\n";
    
    MainWindow* window = new MainWindow(640, 480, false, false, "Hymn to Beauty", false);
    glewInit();
    window->Init(false);
    
    Managers().StartUp();
    
    Hymn().Load(".");
    
    while (!window->ShouldClose()) {
        window->Update();
        Hymn().Render();
        window->SwapBuffers();
        glfwPollEvents();
    }
    
    Managers().ShutDown();
    
    delete window;
    
    glfwTerminate();
    
    Log() << "Game ended - " << time(nullptr) << "\n";
    
    return 0;
}
