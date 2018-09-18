#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Engine/MainWindow.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Input/Input.hpp>
#include <Engine/Util/Input.hpp>
#include <Utility/Log.hpp>
#include <thread>
#include <iostream>
#include <fstream>
#include <ctime>
#include "Util/GameSettings.hpp"

int main(int argc, char* argv[]) {
    if (!glfwInit())
        return 1;

    Log().SetupStreams(&std::cout, &std::cout, &std::cout, &std::cerr);

    Log() << "Game started - " << time(nullptr) << "\n";

    MainWindow* window = new MainWindow(1920, 1080, true, false, "Hymn to Beauty", false);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        Log() << "Failed to initialize OpenGL context\n";
        return -1;
    }

    window->Init(false);
    Input::GetInstance().SetWindow(window->GetGLFWWindow());
    Input()->AssignButton(InputHandler::WINDOWMODE, InputHandler::KEYBOARD, GLFW_KEY_F4);

    Managers().StartUp();

    // Load hymn from json file.
    Hymn().Load(".");

    // Load game settings from ini file.
    GameSettings::GetInstance().Load();
    Managers().renderManager->SetTextureReduction(static_cast<uint16_t>(GameSettings::GetInstance().GetLong("Texture Reduction")));
    Managers().renderManager->SetShadowMapSize(GameSettings::GetInstance().GetLong("Shadow Map Size"));

    // Load world.
    Hymn().world.Load(Hymn().GetPath() + "/" + Hymn().startupScene + ".json");

    // Compile scripts.
    Managers().scriptManager->RegisterInput();
    Managers().scriptManager->BuildAllScripts();

    // Main loop.
    double targetFPS = 60.0;
    double lastTime = glfwGetTime();
    double lastTimeRender = glfwGetTime();
    while (!window->ShouldClose()) {
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        if (Input()->Triggered(InputHandler::WINDOWMODE)) {
            bool fullscreen, borderless;
            window->GetWindowMode(fullscreen, borderless);
            window->SetWindowMode(!fullscreen, borderless);
        }

        window->Update();
        Hymn().Update(static_cast<float>(deltaTime));
        Hymn().Render(RenderManager::MONITOR);

        // Swap buffers and wait until next frame.
        window->SwapBuffers();

        long wait = static_cast<long>((1.0 / targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
        if ( wait > 0 )
            std::this_thread::sleep_for(std::chrono::microseconds(wait));
        lastTimeRender = glfwGetTime();

        // Get input.
        glfwPollEvents();
    }

    // Save game settings.
    GameSettings::GetInstance().Save();

    Hymn().world.Clear();
    Managers().ShutDown();

    delete window;

    glfwTerminate();

    Log() << "Game ended - " << time(nullptr) << "\n";
    return 0;
}
