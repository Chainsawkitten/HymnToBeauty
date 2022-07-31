#include <string>
#include <cassert>
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <Utility/Log.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>

static Engine engine;
static bool started = false;

void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            // The window is being shown, get it ready.
            // Start engine.
            engine.configuration.androidWindow = app->window;
            engine.configuration.graphicsAPI = Video::Renderer::GraphicsAPI::VULKAN;
            engine.Start();

            // Load hymn from json file.
            Hymn().Load(app->activity->externalDataPath);

            // Load world.
            const std::string worldPath = Hymn().GetPath() + "/" + Hymn().startupScene;
            Log(Log::INFO) << "Loading world " << worldPath;
            Hymn().world.Load(worldPath);

            // Compile scripts.
            Managers().scriptManager->RegisterInput();
            Managers().scriptManager->BuildAllScripts();

            started = true;
            break;
        }
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            /// @todo Implement
            engine.Shutdown();
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
            // Draw a frame.
            engine.Update();
            engine.Render();
            engine.Present();
        }
    } while (app->destroyRequested == 0);
}
