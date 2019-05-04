#include <Engine/Engine.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Hymn.hpp>
#include <Utility/Log.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include "Util/GameSettings.hpp"

int main(int argc, char* argv[]) {
    Log().SetupStreams(&std::cout, &std::cout, &std::cout, &std::cerr);

    Log() << "Game started - " << time(nullptr) << "\n";

    // Start engine.
    Engine engine;
    engine.configuration.graphicsAPI = Video::Renderer::GraphicsAPI::OPENGL;
    /// @todo Configure engine based on settings.
    if (!engine.Start())
        return -1;

    // Load hymn from json file.
    Hymn().Load(".");

    // Load game settings from ini file.
    GameSettings::GetInstance().Load();

    // Load world.
    Hymn().world.Load(Hymn().GetPath() + "/" + Hymn().startupScene + ".json");

    // Compile scripts.
    Managers().scriptManager->RegisterInput();
    Managers().scriptManager->BuildAllScripts();

    // Main loop.
    while (!engine.ShouldClose()) {
        engine.Update();
        engine.Render();
        engine.Present();
    }

    // Save game settings.
    GameSettings::GetInstance().Save();

    engine.Shutdown();

    Log() << "Game ended - " << time(nullptr) << "\n";
    return 0;
}
