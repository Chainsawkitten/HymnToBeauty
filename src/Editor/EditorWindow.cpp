#include <GL/glew.h>
#include "EditorWindow.hpp"

#include "Util/EditorSettings.hpp"
#include <Core/Util/Log.hpp>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

const EditorWindow::vec2 EditorWindow::vertices[4] = { { -1.f, 1.f }, { 1.f, 1.f }, { -1.f, -1.f }, { 1.f, -1.f } };
const unsigned int EditorWindow::indices[6] = { 0, 1, 3, 0, 3, 2 };

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
    
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &vertexAttribute);
    
    glfwDestroyWindow(window);
}

void EditorWindow::Init() {
    BindQuad();
    
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
    
    glBindVertexArray(vertexAttribute);
    
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)0);
    
    glfwSwapBuffers(window);
}

void EditorWindow::BindQuad() {
    // Define vertex data layout
    glGenVertexArrays(1, &vertexAttribute);
    glBindVertexArray(vertexAttribute);
    
    // Vertex buffer
    vertexCount = 4;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vec2), vertices, GL_STATIC_DRAW);
    
    // Index buffer
    indexCount = 6;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}
