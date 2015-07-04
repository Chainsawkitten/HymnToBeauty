#ifndef EDITORWINDOW_HPP
#define EDITORWINDOW_HPP

#include <GLFW/glfw3.h>
#include <Core/GameWindow.hpp>
#include <Core/Shader/ShaderProgram.hpp>

/** @ingroup Editor
 * @{
 */

/// Handles the main editor window.
class EditorWindow {
    public:
        /// Create new editor window.
        EditorWindow();

        /// Destructor.
        ~EditorWindow();
        
        /// Initialize components.
        void Init();

        /// Get whether the editor window should close.
        /**
         * @return Whether the editor window should close
         */
        bool ShouldClose() const;

        /// Update the editor.
        void Update();

        /// Render the editor.
        void Render();

    private:
        GLFWwindow* window;
        GameWindow* gameWindow;
        
        struct vec2 {
            float x;
            float y;
        };
        
        // Full screen quad
        static const vec2 vertices[4];
        static const unsigned int indices[6];
        
        // Vertex buffer
        GLuint vertexBuffer;
        GLuint vertexAttribute;
        unsigned int vertexCount;
        
        // Index buffer
        GLuint indexBuffer;
        unsigned int indexCount;
        
        // Shaders
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
        
        void BindQuad();
};

/** @} */

#endif
