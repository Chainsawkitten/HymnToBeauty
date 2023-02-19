#include <Engine/Engine.hpp>
#include "Editor.hpp"
#include "Util/EditorSettings.hpp"
#include <Engine/Util/FileSystem.hpp>
#include <Utility/Log.hpp>
#include <Utility/Profiling/Profiling.hpp>
#include <Utility/Window.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/InputManager.hpp>
#include <Engine/Manager/ProfilingManager.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Hymn.hpp>
#include "ImGui/Implementation.hpp"
#include <imgui.h>
#include "GUI/ProfilingWindow.hpp"
#include <iostream>

int main() {
    // Enable logging to file if requested.
    if (EditorSettings::GetInstance().GetBool("Logging")) {
        Log::SetupFile(FileSystem::DataPath("Hymn to Beauty", "log.txt"));
    }

    Log() << "Editor started - " << time(nullptr) << "\n";

    // Configure engine.
    Engine engine;
    engine.configuration.width = EditorSettings::GetInstance().GetLong("Width");
    engine.configuration.height = EditorSettings::GetInstance().GetLong("Height");
    engine.configuration.fullscreen = false;
    engine.configuration.borderless = false;
    engine.configuration.debug = EditorSettings::GetInstance().GetBool("Debug Context");
    if (EditorSettings::GetInstance().GetString("API") == "Vulkan") {
        engine.configuration.graphicsAPI = Video::Renderer::GraphicsAPI::VULKAN;
    }
    if (EditorSettings::GetInstance().GetString("API") == "WebGPU") {
        engine.configuration.graphicsAPI = Video::Renderer::GraphicsAPI::WEBGPU;
    }

    // Start engine.
    if (!engine.Start())
        return -1;

    // Setup imgui implementation.
    Utility::Window* window = engine.GetWindow();
    ImGuiImplementation::Init(window->GetGLFWWindow(), Managers().renderManager->GetRenderer());

    Editor* editor = new Editor(window, Managers().renderManager->GetRenderer()->GetLowLevelRenderer());

    // Main loop.
    while (!engine.ShouldClose() || !editor->ReadyToClose()) {
        if (Managers().inputManager->Triggered(InputManager::PROFILE)) {
            Managers().profilingManager->SetActive(!Managers().profilingManager->Active());
        }

        // Update engine.
        engine.configuration.paused = editor->IsVisible();
        engine.Update();

        ImGuiImplementation::NewFrame();

        if (editor->IsVisible()) {
            editor->RenderHymn();

            if (window->ShouldClose())
                editor->Close();

            editor->Show(static_cast<float>(engine.GetDeltaTime()));

            if (window->ShouldClose() && !editor->IsClosing())
                window->CancelClose();
        } else {
            engine.Render();

            if (Managers().inputManager->Triggered(InputManager::PLAYTEST)) {
                // Rollback to the editor state.
                editor->LoadSceneState();

                // Turn editor back on.
                editor->SetVisible(true);
            }
        }

        ImGuiImplementation::Render();

        engine.Present();
    }

    // Save editor settings.
    EditorSettings::GetInstance().Save();

    // Shut down and cleanup.
    ImGuiImplementation::Shutdown();
    delete editor;
    engine.Shutdown();

    Log() << "Editor ended - " << time(nullptr) << "\n";
    Log::CloseFile();

    return 0;
}
