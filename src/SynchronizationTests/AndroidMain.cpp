#include <string>
#include <cassert>
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <Utility/Log.hpp>
#include <Utility/Window.hpp>
#include <Framework/Runner.hpp>
#include <Tests/FragFragImage.hpp>
#include <Tests/FragFragBuffer.hpp>
#include <Tests/FragVertImage.hpp>
#include <Tests/FragVertImage2.hpp>
#include <Tests/ComputeFragBuffer.hpp>
#include <Tests/ComputeVertBuffer.hpp>
#include <Tests/MultipleFrames.hpp>
#include <Tests/BarrierMerging.hpp>
#include <Tests/BufferUpload.hpp>
#include <Tests/PrimitiveRestart.hpp>
#include <chrono>

static bool started = false;
Utility::Window* window = nullptr;
FragVertImage2 test;

void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            // The window is being shown, get it ready.
            window = new Utility::Window(app->window);
            SetupTest(&test, window, Video::Renderer::GraphicsAPI::WEBGPU);

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

    uint32_t frame = 0;
    uint32_t frame2 = 0;
    auto start = std::chrono::steady_clock::now();

    // Main loop
    do {
        if (ALooper_pollAll(1, nullptr, &events, (void**)&source) >= 0) {
            if (source != NULL)
                source->process(app, source);
        }

        if (started) {
            RunFrame();

            if (frame == 1) {
                start = std::chrono::steady_clock::now();
            }
            if (frame == 60) {
                auto end = std::chrono::steady_clock::now();
                uint64_t elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                double milli = static_cast<double>(elapsed) / 1000.0;
                Log(Log::INFO) << "60 frames: " << milli << " ms " <<  (60000.0 / milli) << " fps\n";
                frame = 0;
            }
            //if (frame2 == 500) {
            //    Log(Log::INFO) << "Terminate!\n";
            //    GameActivity_finish(app->activity);
            //}
            ++frame;
            ++frame2;
        }
    } while (app->destroyRequested == 0);
}
