#pragma once

#include <GLFW/glfw3.h>
#include <Engine/GameWindow.hpp>
#include <Engine/Font/Font.hpp>
#include "GUI/Container.hpp"
#include "GUI/HorizontalLayout.hpp"
#include "GUI/VerticalLayout.hpp"
#include "GUI/Button.hpp"
#include <Engine/Util/Input.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "GUI/SelectHymnWindow.hpp"

/// Handles the main editor window.
class EditorWindow : public GUI::Container {
    public:
        /// Create new editor window.
        EditorWindow();

        /// Destructor.
        ~EditorWindow() override;
        
        /// Initialize components.
        void Init();

        /// Get whether the editor window should close.
        /**
         * @return Whether the editor window should close
         */
        bool ShouldClose() const;

        /// Update the editor.
        void Update() override;
        
        /// Render the editor.
        void Render();
        
        /// Render the editor.
        /**
         * @param screenSize Size of the screen in pixels.
         */
        void Render(const glm::vec2& screenSize) override;
        
        /// Get the size of the widget.
        /**
         * @return The size
         */
        glm::vec2 Size() const override;
        
        /// Set the size of the widget.
        /**
         * @param size The new size.
         */
        void SetSize(const glm::vec2& size) override;
        
    private:
        void OpenFileMenu();
        void OpenProjectOptions();
        void Play();
        void NewHymn();
        void NewHymnClosed(const std::string& hymn);
        void OpenHymn();
        void OpenHymnClosed(const std::string& hymn);
        
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
        Texture2D* newHymnTexture;
        
        GUI::Button* openHymnButton;
        Texture2D* openHymnTexture;
        
        // Input
        InputHandler* input;
        
        Font* font;
};
