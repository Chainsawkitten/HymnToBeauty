#pragma once

#include <glm/glm.hpp>

#if ANDROID
#include <game-activity/native_app_glue/android_native_app_glue.h>
#else
struct GLFWwindow;
#endif

namespace Utility {

/// 
class Window {
  public:
#if ANDROID
    /// Create new window.
    /**
     * @param androidWindow Android window.
     */
    Window(ANativeWindow* androidWindow);
#else
    /// Create new window.
    /**
     * @param width Width of the window in pixels.
     * @param height Height of the window in pixels.
     * @param fullscreen Whether to start in fullscreen mode.
     * @param borderless Whether to use a borderless window.
     * @param title Window title.
     * @param noAPI Create window without any specific API.
     */
    Window(unsigned int width, unsigned int height, bool fullscreen = false, bool borderless = false, const char* title = "", bool noAPI = false);
#endif

    /// Destructor.
    ~Window();

    /// Set the size of the window.
    /**
     * Note that this function does not resize the window. It merely updates the stored size value.
     * 
     * @param size The new size of the window.
     */
    void SetSize(const glm::uvec2& size);

    /// Get the size of the window.
    /**
     * @return The size of the window in pixels.
     */
    const glm::uvec2& GetSize() const;

    /// Update
    void Update();

    /// Get whether the window should close.
    /**
     * @return Whether the window should close
     */
    bool ShouldClose() const;

    /// Cancel the closing of the window.
    void CancelClose();

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

#if ANDROID
    /// Get Android window.
    /**
     * @return The Android window.
     */
    ANativeWindow* GetAndroidWindow();
#else
    /// Get the GLFW window.
    /**
     * @return The GLFW window.
     */
    GLFWwindow* GetGLFWWindow();
#endif

  private:
    Window(Window& window) = delete;

#if ANDROID
    ANativeWindow* window;
#else
    GLFWwindow* window;
#endif

    glm::uvec2 size;
    bool shouldClose = false;
};

}
