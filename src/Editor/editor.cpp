#include "editor.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "EditorWindow.hpp"
#include "Util/EditorSettings.hpp"
#include <Core/Util/FileSystem.hpp>
#include <Core/Util/Log.hpp>

int main() {
    if (!glfwInit())
        return 1;

    EditorWindow* editorWindow = new EditorWindow();
    glewInit();
    
    // Enable logging if requested.
    if (EditorSettings::GetInstance().GetBool("Logging"))
        freopen(FileSystem::SavePath("Hymn to Beauty", "log.txt").c_str(), "a", stderr);
    
    // Setup error callbacks.
    glfwSetErrorCallback(ErrorCallback);
    if (EditorSettings::GetInstance().GetBool("Debug Context"))
        glDebugMessageCallback(DebugMessageCallback, nullptr);

    while (!editorWindow->ShouldClose()) {
        editorWindow->Update();
        editorWindow->Render();
        glfwPollEvents();
    }

    delete editorWindow;

    glfwTerminate();
    
    EditorSettings::GetInstance().Save();

    return 0;
}
