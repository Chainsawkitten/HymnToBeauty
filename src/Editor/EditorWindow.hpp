#ifndef EDITORWINDOW_HPP
#define EDITORWINDOW_HPP

#include <GLFW/glfw3.h>
#include <Core/GameWindow.hpp>
#include <Core/GUI/Container.hpp>
#include <Core/GUI/HorizontalLayout.hpp>

/** @ingroup Editor
 * @{
 */

/// Handles the main editor window.
class EditorWindow : public GUI::Container {
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
        
        /// Render the editor.
        /**
         * @param screenWidth Width of the screen in pixels.
         * @param screenHeight Height of the screen in pixels.
         */
        void Render(int screenWidth, int screenHeight);

    private:
        GLFWwindow* window;
        GameWindow* gameWindow;
        
        GUI::HorizontalLayout* menuBar;
};

/** @} */

#endif
