#ifndef EDITORWINDOW_HPP
#define EDITORWINDOW_HPP

#include <GLFW/glfw3.h>

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
};

/** @} */

#endif
