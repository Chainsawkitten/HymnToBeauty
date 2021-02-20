#include "Engine.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/DebugDrawingManager.hpp>
#include <Engine/MainWindow.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Util/Profiling.hpp>
#include <Utility/Log.hpp>
#include <thread>
#include <GLFW/glfw3.h>

bool Engine::Start() {
    if (!glfwInit())
        return false;

    window = new MainWindow(configuration.width, configuration.height, configuration.fullscreen, configuration.borderless, "Hymn to Beauty",
                            configuration.graphicsAPI == Video::Renderer::GraphicsAPI::VULKAN);

    Input::GetInstance().SetWindow(window->GetGLFWWindow());
    Input()->AssignButton(InputHandler::WINDOWMODE, InputHandler::KEYBOARD, GLFW_KEY_F4);

    Managers().StartUp(configuration.graphicsAPI);

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
    Managers().profilingManager->BeginFrame();

    {
        PROFILE("Update");

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
            Managers().debugDrawingManager->Update(static_cast<float>(deltaTime));
        }
    }
}

double Engine::GetDeltaTime() const {
    return deltaTime;
}

void Engine::Render() {
    PROFILE("Render");

    Hymn().Render();
}

void Engine::Present() {
    {
        PROFILE("Present");

        // Swap buffers and wait until next frame.
        if (window->GetSize().x > 0 && window->GetSize().y > 0) {
            Managers().renderer->Present();
            Managers().profilingManager->FetchGPUTimeline();
        }

        // Wait for next frame.
        long wait = static_cast<long>((1.0 / configuration.targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
        if (wait > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(wait));
        lastTimeRender = glfwGetTime();
    }

    Managers().profilingManager->EndFrame();
}

void Engine::Shutdown() {
    Managers().renderManager->GetRenderer()->WaitForRender();

    Hymn().world.Clear();
    Managers().ShutDown();

    delete window;

    glfwTerminate();
}
