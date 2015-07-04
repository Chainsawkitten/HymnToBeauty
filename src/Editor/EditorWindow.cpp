#include <GL/glew.h>
#include "EditorWindow.hpp"

#include "Util/EditorSettings.hpp"
#include <Core/Util/Log.hpp>

EditorWindow::EditorWindow() {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Enable debug context and set message callback.
    if (EditorSettings::GetInstance().GetBool("Debug Context"))
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
    window = glfwCreateWindow(640, 480, "Hymn to Beauty", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    glfwMakeContextCurrent(window);

    gameWindow = nullptr;
}

EditorWindow::~EditorWindow() {
    delete vertexShader;
    delete fragmentShader;
    delete shaderProgram;
    
    delete rectangle;
    
    glfwDestroyWindow(window);
}

void EditorWindow::Init() {
    rectangle = new Geometry::Rectangle();
    
    vertexShader = new Shader("#version 400\nlayout(location = 0) in vec2 vertex_position;\nvoid main () {\ngl_Position = vec4(vertex_position, 0.0, 1.0);\n}", 126, GL_VERTEX_SHADER);
    fragmentShader = new Shader("#version 400\nvoid main () {\ngl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n}", 73, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
}

bool EditorWindow::ShouldClose() const {
    return (glfwWindowShouldClose(window) != 0);
}

void EditorWindow::Update() {
    // Handle running game.
    if (gameWindow != nullptr) {
        gameWindow->Update();
        if (gameWindow->ShouldClose()) {
            delete gameWindow;
            gameWindow = nullptr;
        }
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        gameWindow = new GameWindow();
    }
}

void EditorWindow::Render() {
    if (gameWindow != nullptr)
        gameWindow->Render();

    glfwMakeContextCurrent(window);
    
    shaderProgram->Use();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindVertexArray(rectangle->VertexArray());
    
    glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    glfwSwapBuffers(window);
}
