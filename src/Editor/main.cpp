#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/MainWindow.hpp>
#include "Editor.hpp"
#include "Util/EditorSettings.hpp"
#include <Engine/Util/Input.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Utility/Log.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/ProfilingManager.hpp>
#include <Engine/Manager/ParticleManager.hpp>
#include <Engine/Manager/DebugDrawingManager.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Util/Profiling.hpp>
#include <Engine/Util/GPUProfiling.hpp>
#include <Engine/Hymn.hpp>
#include <thread>
#include "ImGui/OpenGLImplementation.hpp"
#include <imgui.h>
#include "GUI/ProfilingWindow.hpp"
#include <iostream>

int main() {
    Log().SetupStreams(&std::cout, &std::cout, &std::cout, &std::cerr);

    // Enable logging if requested.
    if (EditorSettings::GetInstance().GetBool("Logging")){
        FILE* file = freopen(FileSystem::DataPath("Hymn to Beauty", "log.txt").c_str(), "a", stderr);
        if (file == nullptr)
            Log() << "Could not open logging file!\n";
    }

    Log() << "Editor started - " << time(nullptr) << "\n";

    if (!glfwInit())
        return 1;

    MainWindow* window = new MainWindow(EditorSettings::GetInstance().GetLong("Width"), EditorSettings::GetInstance().GetLong("Height"), false, false, "Hymn to Beauty", EditorSettings::GetInstance().GetBool("Debug Context"));

    glewInit();
    window->Init(false);

    Input::GetInstance().SetWindow(window->GetGLFWWindow());

    Managers().StartUp();

    Editor* editor = new Editor();
    // Setup imgui implementation.
    ImGuiImplementation::Init(window->GetGLFWWindow());

    bool profiling = false;
    GUI::ProfilingWindow profilingWindow;

    // Main loop.
    double targetFPS = 60.0;
    double lastTime = glfwGetTime();
    double lastTimeRender = glfwGetTime();
    while (!window->ShouldClose() || !editor->ReadyToClose()) {
        float deltaTime = static_cast<float>(glfwGetTime() - lastTime);
        lastTime = glfwGetTime();

        Managers().profilingManager->SetActive(profiling);

        // Begin new profiling frame.
        if (Managers().profilingManager->Active())
            Managers().profilingManager->BeginFrame();

        { PROFILE("Frame");
        { GPUPROFILE("Frame", Video::Query::Type::TIME_ELAPSED);

            glfwPollEvents();

            if (Input()->Triggered(InputHandler::WINDOWMODE)) {
                bool fullscreen, borderless;
                window->GetWindowMode(fullscreen, borderless);
                window->SetWindowMode(!fullscreen, borderless);
            }

            if (Input()->Triggered(InputHandler::PROFILE))
                profiling = !profiling;

            // Start new frame.
            ImGuiImplementation::NewFrame();

            window->Update();

            if (editor->IsVisible()) {
                Hymn().world.ClearKilled();
                Managers().particleManager->Update(Hymn().world, deltaTime, true);

                Managers().debugDrawingManager->Update(deltaTime);
                Hymn().Render(RenderManager::MONITOR, editor->GetCamera(),
                              EditorSettings::GetInstance().GetBool("Sound Source Icons"),
                              EditorSettings::GetInstance().GetBool("Particle Emitter Icons"),
                              EditorSettings::GetInstance().GetBool("Light Source Icons"),
                              EditorSettings::GetInstance().GetBool("Camera Icons"),
                              EditorSettings::GetInstance().GetBool("Physics Volumes"),
                              EditorSettings::GetInstance().GetBool("Lighting"),
                              EditorSettings::GetInstance().GetBool("Light Volumes"));

                if (window->ShouldClose())
                    editor->Close();

                editor->Show(deltaTime);

                if (window->ShouldClose() && !editor->isClosing())
                    window->CancelClose();

            } else {
                { PROFILE("Update");
                { GPUPROFILE("Update", Video::Query::Type::TIME_ELAPSED);
                    Hymn().Update(deltaTime);
                }
                }

                { PROFILE("Render");
                { GPUPROFILE("Render", Video::Query::Type::TIME_ELAPSED);
                    Hymn().Render(RenderManager::MONITOR);
                }
                }

                if (Input()->Triggered(InputHandler::PLAYTEST)) {
                    // Rollback to the editor state.
                    editor->LoadSceneState();

                    // Turn editor back on.
                    editor->SetVisible(true);
                }
            }
        }
        }

        if (Managers().profilingManager->Active()) {
            Managers().profilingManager->EndFrame();
            profilingWindow.Show();
        }

        ImGui::Render();

        // Swap buffers and wait until next frame.
        window->SwapBuffers();

        long wait = static_cast<long>((1.0 / targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
        if (wait > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(wait));
        lastTimeRender = glfwGetTime();
    }

    // Save editor settings.
    EditorSettings::GetInstance().Save();

    // Shut down and cleanup.
    ImGuiImplementation::Shutdown();
    delete editor;
    Hymn().world.Clear();

    Managers().ShutDown();

    delete window;
    glfwTerminate();

    Log() << "Editor ended - " << time(nullptr) << "\n";

    return 0;
}
