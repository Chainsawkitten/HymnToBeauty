#include "Engine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ParticleManager.hpp>
#include <Engine/Manager/DebugDrawingManager.hpp>
#include <Engine/MainWindow.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Util/Profiling.hpp>
#include <Engine/Util/GPUProfiling.hpp>
#include <Utility/Log.hpp>
#include <thread>

bool Engine::Start() {
    if (!glfwInit())
        return false;

    window = new MainWindow(configuration.width, configuration.height, configuration.fullscreen, configuration.borderless, "Hymn to Beauty", configuration.debug);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Log() << "Failed to initialize OpenGL context\n";
        return false;
    }

    window->Init(false);
    Input::GetInstance().SetWindow(window->GetGLFWWindow());
    Input()->AssignButton(InputHandler::WINDOWMODE, InputHandler::KEYBOARD, GLFW_KEY_F4);

    Managers().StartUp();

    // Main loop.
    lastTime = glfwGetTime();
    lastTimeRender = glfwGetTime();

    return true;
}

MainWindow* Engine::GetMainWindow() {
    return window;
}

bool Engine::ShouldClose() const {
    return window->ShouldClose();
}

void Engine::Update() {
    PROFILE("Update");
    GPUPROFILE("Update", Video::Query::Type::TIME_ELAPSED);

    deltaTime = glfwGetTime() - lastTime;
    lastTime = glfwGetTime();

    // Switch between fullscreen and window mode.
    if (Input()->Triggered(InputHandler::WINDOWMODE)) {
        bool fullscreen, borderless;
        window->GetWindowMode(fullscreen, borderless);
        window->SetWindowMode(!fullscreen, borderless);
    }

    // Update input.
    glfwPollEvents();
    window->Update();

    // Update hymn.
    if (!configuration.paused) {
        Hymn().Update(static_cast<float>(deltaTime));
    } else {
        Hymn().world.ClearKilled();
        Managers().particleManager->Update(Hymn().world, deltaTime, true);
        Managers().debugDrawingManager->Update(deltaTime);
    }
}

double Engine::GetDeltaTime() const {
    return deltaTime;
}

void Engine::Render() {
    PROFILE("Render");
    GPUPROFILE("Render", Video::Query::Type::TIME_ELAPSED);

    Hymn().Render();
}

void Engine::Present() {
    // Swap buffers and wait until next frame.
    window->SwapBuffers();

    // Wait for next frame.
    long wait = static_cast<long>((1.0 / configuration.targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
    if (wait > 0)
        std::this_thread::sleep_for(std::chrono::microseconds(wait));
    lastTimeRender = glfwGetTime();
}

void Engine::Shutdown() {
    Hymn().world.Clear();
    Managers().ShutDown();

    delete window;

    glfwTerminate();
}
