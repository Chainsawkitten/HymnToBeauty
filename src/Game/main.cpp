#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/MainWindow.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/Log.hpp>
#include <thread>

int main() {
    if (!glfwInit())
        return 1;
    
    Log() << "Game started - " << time(nullptr) << "\n";
    
    MainWindow* window = new MainWindow(640, 480, false, false, "Hymn to Beauty", false);
    glewInit();
    window->Init(false);
    
    Managers().StartUp();
    
    Hymn().Load(".");
    
    // Main loop.
    double targetFPS = 60.0;
    double lastTime = glfwGetTime();
    double lastTimeRender = glfwGetTime();
    while (!window->ShouldClose()) {
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        
        window->Update();
        Hymn().Update(static_cast<float>(deltaTime));
        Hymn().Render();
        
        // Swap buffers and wait until next frame.
        window->SwapBuffers();
        
        long wait = static_cast<long>((1.0 / targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
        if (wait > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(wait));
        lastTimeRender = glfwGetTime();
        
        // Get input.
        glfwPollEvents();
    }
    
    Managers().ShutDown();
    
    delete window;
    
    glfwTerminate();
    
    Log() << "Game ended - " << time(nullptr) << "\n";
    
    return 0;
}
