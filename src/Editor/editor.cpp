#include "editor.hpp"
#include <GLFW/glfw3.h>
#include "EditorWindow.hpp"

int main() {
    if (!glfwInit())
        return 1;

    EditorWindow* editorWindow = new EditorWindow();

    while (!editorWindow->ShouldClose()) {
        editorWindow->Update();
        editorWindow->Render();
        glfwPollEvents();
    }

    delete editorWindow;

    glfwTerminate();

    return 0;
}
