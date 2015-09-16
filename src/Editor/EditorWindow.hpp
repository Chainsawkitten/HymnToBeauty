#ifndef EDITORWINDOW_HPP
#define EDITORWINDOW_HPP

#include <GLFW/glfw3.h>
#include <Core/GameWindow.hpp>
#include <Core/Font/Font.hpp>
#include "GUI/Container.hpp"
#include "GUI/HorizontalLayout.hpp"
#include "GUI/VerticalLayout.hpp"
#include "GUI/Button.hpp"
#include <Core/Util/Input.hpp>
#include <Core/Texture/Texture2D.hpp>
#include "GUI/SelectHymnWindow.hpp"

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
         * @param screenSize Size of the screen in pixels.
         */
        void Render(const glm::vec2& screenSize);
        
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
        
        /// Create new hymn.
        void NewHymn();
        
        /// Callback for when New Hymn window is closed.
        void NewHymnClosed();
        
        /// Open an existing hymn.
        void OpenHymn();
        
        /// Callback for when Open Hymn window is closed.
        void OpenHymnClosed();
        
        /// Save the currently open hymn.
        void SaveHymn();
        
    private:
        GLFWwindow* window;
        GameWindow* gameWindow;
        GUI::SelectHymnWindow* childWindow;
        
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
        
        Font* font;
};

/** @} */

#endif
