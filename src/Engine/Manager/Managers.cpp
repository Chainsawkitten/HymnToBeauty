#include "Managers.hpp"

#include <Video/Renderer.hpp>
#include "ResourceManager.hpp"
#include "InputManager.hpp"
#include "RenderManager.hpp"
#include "PhysicsManager.hpp"
#include "SoundManager.hpp"
#include "ScriptManager.hpp"
#include "DebugDrawingManager.hpp"
#include "ProfilingManager.hpp"
#include "TriggerManager.hpp"

#include <Utility/Log.hpp>

Hub::Hub() {}

Hub& Managers() {
    static Hub instance;

    return instance;
}

void Hub::StartUp(Video::Renderer::GraphicsAPI graphicsAPI, Utility::Window* window) {
    renderer = new Video::Renderer(graphicsAPI, window);

    resourceManager = new ResourceManager(renderer);
    inputManager = new InputManager(window);
    renderManager = new RenderManager(renderer);
    physicsManager = new PhysicsManager();
    soundManager = new SoundManager();
    scriptManager = new ScriptManager(window);
    debugDrawingManager = new DebugDrawingManager();
    profilingManager = new ProfilingManager();
    triggerManager = new TriggerManager();
}

void Hub::ShutDown() {
    delete triggerManager;
    delete profilingManager;
    delete debugDrawingManager;
    delete scriptManager;
    delete soundManager;
    delete physicsManager;
    delete renderManager;
    delete inputManager;
    delete resourceManager;

    delete renderer;

    shutdown = true;
}

void Hub::ClearKilledComponents() {
    if (!shutdown) {
        triggerManager->ClearKilledComponents();
        renderManager->ClearKilledComponents();
        physicsManager->ClearKilledComponents();
        soundManager->ClearKilledComponents();
        scriptManager->ClearKilledComponents();
    }
}
