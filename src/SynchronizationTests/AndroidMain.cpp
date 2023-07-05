#include <string>
#include <cassert>
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <Utility/Log.hpp>
#include <Utility/Window.hpp>
#include <Framework/Runner.hpp>
#include <Tests/FragFragImage.hpp>
#include <Tests/FragFragBuffer.hpp>

static bool started = false;
Utility::Window* window = nullptr;
FragFragBuffer test;

void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            // The window is being shown, get it ready.
            window = new Utility::Window(app->window);
            SetupTest(&test, window, Video::Renderer::GraphicsAPI::VULKAN);

            started = true;
            break;
        }
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            ShutdownTest();
            delete window;
            started = false;
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    // Set the callback to process system events
    app->onAppCmd = handle_cmd;

    // Used to poll the events in the main loop
    int events;
    android_poll_source* source;

    // Main loop
    do {
        if (ALooper_pollAll(1, nullptr, &events, (void**)&source) >= 0) {
            if (source != NULL)
                source->process(app, source);
        }

        if (started) {
            RunFrame();
        }
    } while (app->destroyRequested == 0);
}
