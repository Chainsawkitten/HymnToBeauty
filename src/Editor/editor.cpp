#include "editor.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "EditorWindow.hpp"

int main() {
    if (!glfwInit())
        return 1;

    EditorWindow* editorWindow = new EditorWindow();
    glewInit();

    while (!editorWindow->ShouldClose()) {
        editorWindow->Update();
        editorWindow->Render();
        glfwPollEvents();
    }

    delete editorWindow;

    glfwTerminate();

    return 0;
}
