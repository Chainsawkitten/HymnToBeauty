#include "Engine.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/InputManager.hpp>
#include <Engine/Manager/DebugDrawingManager.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Manager/ProfilingManager.hpp>
#include <Engine/Input/Input.hpp>
#include <Utility/Profiling/Profiling.hpp>
#include <Utility/Log.hpp>
#include <Utility/Time.hpp>
#include <Utility/Window.hpp>
#include <thread>
#if !ANDROID
#include <GLFW/glfw3.h>
#endif

bool Engine::Start() {
#if !ANDROID
    if (!glfwInit())
        return false;
#endif
    Utility::InitTime();

#if ANDROID
    window = new Utility::Window(configuration.androidWindow);
#else
    window = new Utility::Window(configuration.width, configuration.height, configuration.fullscreen, configuration.borderless, "Hymn to Beauty",
                                 configuration.graphicsAPI == Video::Renderer::GraphicsAPI::VULKAN || configuration.graphicsAPI == Video::Renderer::GraphicsAPI::WEBGPU);
#endif

    Input::GetInstance().SetWindow(window);

    Managers().StartUp(configuration.graphicsAPI, window);
#if !ANDROID
    Managers().inputManager->AssignButton(InputManager::WINDOWMODE, InputManager::KEYBOARD, GLFW_KEY_F4);
#endif

    // Main loop.
    lastTime = Utility::GetTime();
    lastTimeRender = Utility::GetTime();

    return true;
}

Utility::Window* Engine::GetWindow() {
    return window;
}

bool Engine::ShouldClose() const {
    return window->ShouldClose();
}

void Engine::Update() {
    Managers().profilingManager->BeginFrame();

    {
        PROFILE("Update");

        deltaTime = Utility::GetTime() - lastTime;
        lastTime = Utility::GetTime();

#if !ANDROID
        // Switch between fullscreen and window mode.
        if (Managers().inputManager->Triggered(InputManager::WINDOWMODE)) {
            bool fullscreen, borderless;
            window->GetWindowMode(fullscreen, borderless);
            window->SetWindowMode(!fullscreen, borderless);
        }
#endif

        // Update input.
        Managers().inputManager->Update();
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

    RenderManager::DebugConfiguration debugConfiguration = {};
    Hymn().Render(debugConfiguration);
}

void Engine::Present() {
    {
        PROFILE("Present");

        // Swap buffers and wait until next frame.
        if (window->GetSize().x > 0 && window->GetSize().y > 0) {
            Managers().renderer->Present();
            Managers().profilingManager->FetchGPUTimeline();
        }
    }

    {
        PROFILE("Wait");

        // Wait for next frame.
        /// @todo Make limiting FPS optional.
        while (Utility::GetTime() < lastTimeRender + 1.0 / configuration.targetFPS)
            std::this_thread::yield();
        lastTimeRender = Utility::GetTime();
    }

    Managers().profilingManager->EndFrame();
}

void Engine::Shutdown() {
    Managers().renderManager->GetRenderer()->WaitForRender();

    Hymn().world.Clear();
    Managers().ShutDown();

    delete window;

#if !ANDROID
    glfwTerminate();
#endif
}
