#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Editor.hpp"
#include "Util/EditorSettings.hpp"
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Util/Log.hpp>
#include <Engine/Manager/Managers.hpp>

int main() {
    // Enable logging if requested.
    if (EditorSettings::GetInstance().GetBool("Logging"))
        freopen(FileSystem::DataPath("Hymn to Beauty", "log.txt").c_str(), "a", stderr);
    
    Log() << "Editor started - " << time(nullptr) << "\n";
    
    if (!glfwInit())
        return 1;
    
    Editor* editor = new Editor();
    glewInit();
    
    // Setup error callbacks.
    glfwSetErrorCallback(ErrorCallback);
    if (EditorSettings::GetInstance().GetBool("Debug Context"))
        glDebugMessageCallback(DebugMessageCallback, nullptr);
    
    Managers().StartUp(editor->GetSize());
    
    editor->Init();
    
    while (!editor->ShouldClose()) {
        editor->Update();
        editor->Render();
        glfwPollEvents();
    }
    
    editor->Save();
    delete editor;
    
    Managers().ShutDown();
    
    glfwTerminate();
    
    EditorSettings::GetInstance().Save();
    
    Log() << "Editor ended - " << time(nullptr) << "\n";
    
    return 0;
}
