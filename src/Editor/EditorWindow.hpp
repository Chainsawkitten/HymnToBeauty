#ifndef EDITORWINDOW_HPP
#define EDITORWINDOW_HPP

#include <GLFW/glfw3.h>
#include <Core/GameWindow.hpp>
#include <Core/GUI/Font.hpp>
#include <Core/GUI/Container.hpp>
#include <Core/GUI/HorizontalLayout.hpp>
#include <Core/GUI/VerticalLayout.hpp>
#include <Core/GUI/Button.hpp>
#include <Core/Util/Input.hpp>
#include <Core/Texture/Texture2D.hpp>

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
        
        /// Get the size of the widget.
        /**
         * @return The size
         */
        glm::vec2 Size() const;
        
        /// Open the file menu.
        void OpenFileMenu();
        
        /// Open the project options window.
        void OpenProjectOptions();

        /// Play game.
        void Play();
    private:
        
        GLFWwindow* window;
        GameWindow* gameWindow;
        
        // Menu bar.
        GUI::HorizontalLayout* menuBar;
        GUI::Button* fileButton;
        Texture2D* fileTexture;
        
        GUI::Button* optionsButton;
        Texture2D* optionsTexture;
        
        GUI::Button* playButton;
        Texture2D* playTexture;
        
        // File menu
        GUI::VerticalLayout* fileMenu;
        GUI::Button* newHymnButton;
        GUI::Button* openHymnButton;
        GUI::Button* saveHymnButton;
        
        // Input
        InputHandler* input;
        
        GUI::Font* font;
};

/** @} */

#endif
