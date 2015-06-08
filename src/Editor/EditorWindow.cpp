#include "EditorWindow.hpp"

EditorWindow::EditorWindow() {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(640, 480, "Hymn to Beauty", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    glfwMakeContextCurrent(window);
}

EditorWindow::~EditorWindow() {
    glfwDestroyWindow(window);
}

bool EditorWindow::ShouldClose() const {
    return (glfwWindowShouldClose(window) != 0);
}

void EditorWindow::Update() {

}

void EditorWindow::Render() {
    glfwSwapBuffers(window);
}
