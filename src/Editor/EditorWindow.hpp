#ifndef EDITORWINDOW_HPP
#define EDITORWINDOW_HPP

#include <GLFW/glfw3.h>
#include <Core/GameWindow.hpp>
#include <Core/Shader/ShaderProgram.hpp>
#include <Core/Geometry/Rectangle.hpp>

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
        
        Geometry::Rectangle* rectangle;
        
        // Shaders
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};

/** @} */

#endif
