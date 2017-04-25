#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/MainWindow.hpp>
#include "Editor.hpp"
#include "Util/EditorSettings.hpp"
#include <Engine/Util/Input.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Util/Log.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/DebugDrawingManager.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/ProfilingManager.hpp>
#include <Engine/Util/Profiling.hpp>
#include <Engine/Hymn.hpp>
#include <thread>
#include "ImGui/OpenGLImplementation.hpp"
#include <imgui.h>

int main() {
    // Enable logging if requested.
    if (EditorSettings::GetInstance().GetBool("Logging"))
        freopen(FileSystem::DataPath("Hymn to Beauty", "log.txt").c_str(), "a", stderr);
    
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
    
    // Test debug drawing facilites.
    Managers().debugDrawingManager->AddPoint(glm::vec3(3.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 1.f), 10.f, 20.f, false);
    
    bool profiling = false;
    
    // Main loop.
    double targetFPS = 60.0;
    double lastTime = glfwGetTime();
    double lastTimeRender = glfwGetTime();
    while (!window->ShouldClose()) {
        float deltaTime = static_cast<float>(glfwGetTime() - lastTime);
        lastTime = glfwGetTime();
        
        // Begin new profiling frame.
        Managers().profilingManager->BeginFrame();
        
        { PROFILE("Frame");
            glfwPollEvents();
            
            if (Input()->Triggered(InputHandler::PROFILE))
                profiling = !profiling;
            
            // Start new frame.
            ImGuiImplementation::NewFrame();
            
            window->Update();
            
            if (editor->IsVisible()) {
                Hymn().world.ClearKilled();
                Hymn().Render(editor->GetCamera(), EditorSettings::GetInstance().GetBool("Sound Source Icons"), EditorSettings::GetInstance().GetBool("Particle Emitter Icons"), EditorSettings::GetInstance().GetBool("Light Source Icons"), EditorSettings::GetInstance().GetBool("Camera Icons"));
                
                editor->Show(deltaTime);
            } else {
                { PROFILE("Update");
                    Hymn().Update(deltaTime);
                }
                { PROFILE("Render");
                    Hymn().Render();
                }
                
                if (Input()->Triggered(InputHandler::PLAYTEST)) {
                    // Reload hymn.
                    std::string path = Hymn().GetPath();
                    Hymn().Load(path);
                    
                    // Turn editor back on.
                    editor->SetVisible(true);
                }
            }
        }
        
        if (profiling)
            Managers().profilingManager->ShowResults();
        
        ImGui::Render();
        
        // Swap buffers and wait until next frame.
        window->SwapBuffers();
        
        long wait = static_cast<long>((1.0 / targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
        if (wait > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(wait));
        lastTimeRender = glfwGetTime();
    }
    
    // Save.
    EditorSettings::GetInstance().Save();
    if (editor->IsVisible())
        editor->Save();
    
    // Shut down and cleanup.
    ImGuiImplementation::Shutdown();
    delete editor;
    
    Managers().ShutDown();
    
    delete window;
    glfwTerminate();
    
    Log() << "Editor ended - " << time(nullptr) << "\n";
    
    return 0;
}
