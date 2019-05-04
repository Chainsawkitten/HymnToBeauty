#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;
class InputHandler;

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
     * @param noAPI Create window without any specific API.
     */
    MainWindow(unsigned int width, unsigned int height, bool fullscreen = false, bool borderless = false, const char* title = "", bool noAPI = false);

    /// Destructor.
    ~MainWindow();

    /// Get the instance of %MainWindow.
    /**
     * @return The %MainWindow instance.
     */
    static MainWindow* GetInstance();

    /// Update
    void Update();

    /// Get the size of the window.
    /**
     * @return The size of the window in pixels.
     */
    const glm::uvec2& GetSize() const;

    /// Set the size of the window.
    /**
     * @param width The width of the window in pixels.
     * @param height The height of the window in pixels.
     */
    void SetSize(unsigned int width, unsigned int height);

    /// Set window title.
    /**
     * @param title New window title.
     */
    void SetTitle(const char* title) const;

    /// Get whether the window should close.
    /**
     * @return Whether the window should close
     */
    bool ShouldClose() const;

    /// Close the window.
    void Close();

    /// Cancel the closing of the window.
    void CancelClose();

    /// Get GLFW window.
    /**
     * @return The GLFW window struct.
     */
    GLFWwindow* GetGLFWWindow() const;

    /// Set window mode.
    /**
     * @param fullscreen Whether window should be fullscreen.
     * @param borderless Whether window should be borderless.
     */
    void SetWindowMode(bool fullscreen, bool borderless) const;

    /// Get window mode.
    /**
     * @param fullscreen Get whether window is fullscreen.
     * @param borderless Get whether window is borderless.
     */
    void GetWindowMode(bool& fullscreen, bool& borderless) const;

  private:
    // Copy Constructor
    MainWindow(MainWindow& mainwindow) = delete;

    static MainWindow* instance;

    InputHandler* input;
    GLFWwindow* window;
    glm::uvec2 size;

    bool shouldClose = false;
};
