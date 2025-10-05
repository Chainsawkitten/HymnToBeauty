#pragma once

#include <Video/Renderer.hpp>

#if __ANDROID__
#include <game-activity/native_app_glue/android_native_app_glue.h>
#endif

/// An instance of the engine.
class Engine {
  public:
    /// Start the engine based on the Configuration.
    /**
     * Sets up libraries, creates a window and starts managers.
     *
     * @return Whether startup was successful.
     */
    bool Start();

    /// Get the window.
    /**
     * @return The window.
     */
    Utility::Window* GetWindow();

    /// Get whether the application should close.
    /**
     * @return Whether the application should close.
     */
    bool ShouldClose() const;

    /// Update the engine.
    /**
     * Updates the moving parts of the engine.
     */
    void Update();

    /// Get time between frames.
    /**
     * @return The time in seconds since last frame.
     */
    double GetDeltaTime() const;

    /// Render a frame.
    void Render();

    /// Swap buffers and wait for next frame.
    void Present();

    /// Shut down the engine.
    void Shutdown();

    /// Engine configuration.
    struct Configuration {
#if __ANDROID__
        /// The native window to render in.
        ANativeWindow* androidWindow;
#else
        /// Window width.
        int width = 1024;

        /// Window height.
        int height = 768;

        /// Whether to run in fullscreen mode.
        bool fullscreen = false;

        /// Whether to run in borderless window mode.
        bool borderless = false;
#endif

        /// Whether to run in debug context.
        bool debug = false;

        /// The target FPS.
        double targetFPS = 60.0;

        /// Whether the hymn is paused.
        /**
         * Only certain things will be updated. Used in the editor.
         */
        bool paused = false;

        /// Which graphics API to use for rendering.
        Video::Renderer::GraphicsAPI graphicsAPI = Video::Renderer::GraphicsAPI::OPENGL;
    } configuration;

  private:
    Utility::Window* window = nullptr;

    double deltaTime = 0.0;
    double lastTime;
    double lastTimeRender;
};
