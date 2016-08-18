#pragma once

#include "Util/Input.hpp"
#include <glm/glm.hpp>

struct GLFWwindow;

/// The main game window in which the game is contained.
class MainWindow {
    public:
        /// Create new main window.
        /**
         * @param width Width of the window in pixels.
         * @param height Height of the window in pixels.
         * @param fullscreen Whether to start in fullscreen mode.
         * @param borderless Whether to use a borderless window.
         * @param title Window title.
         * @param debugContext Whether to activate OpenGL debug context.
         */
        MainWindow(int width, int height, bool fullscreen = false, bool borderless = false, const char* title = "", bool debugContext = false);
        
        /// Destructor.
        ~MainWindow();
        
        /// Get the instance of %MainWindow.
        /**
         * @return The %MainWindow instance.
         */
        static MainWindow* GetInstance();
        
        /// Initialize components.
        /**
         * @param showNotifications Whether to show debug messages of notification priority.
         */
        void Init(bool showNotifications = false);
        
        /// Set whether vsync is enabled.
        /**
         * @param vsync Whether to enable vsync.
         */
        void SetVsync(bool vsync);

        /// Update
        void Update();
        
        /// Get the size of the window.
        /**
         * @return The size of the window in pixels.
         */
        const glm::vec2& GetSize() const;
        
        /// Set window title.
        /**
         * @param title New window title.
         */
        void SetTitle(const char* title);
        
        /// Get whether the window should close.
        /**
         * @return Whether the window should close
         */
        bool ShouldClose() const;
        
        /// Close the window.
        void Close();
        
        /// Swap front- and backbuffers.
        void SwapBuffers();
        
    private:
        static MainWindow* instance;
        
        InputHandler* input;
        GLFWwindow* window;
        bool debugContext;
        glm::vec2 size;
        
        bool shouldClose = false;
};
