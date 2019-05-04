#include "Managers.hpp"

#include <Video/Renderer.hpp>
#include "ResourceManager.hpp"
#include "RenderManager.hpp"
#include "PhysicsManager.hpp"
#include "SoundManager.hpp"
#include "ScriptManager.hpp"
#include "DebugDrawingManager.hpp"
#include "ProfilingManager.hpp"
#include "TriggerManager.hpp"

#include "../MainWindow.hpp"
#include <Utility/Log.hpp>

Hub::Hub() {}

Hub& Managers() {
    static Hub instance;

    return instance;
}

void Hub::StartUp(Video::Renderer::GraphicsAPI graphicsAPI) {
    renderer = new Video::Renderer(graphicsAPI, MainWindow::GetInstance()->GetGLFWWindow());

    resourceManager = new ResourceManager(renderer);
    renderManager = new RenderManager(renderer);
    physicsManager = new PhysicsManager();
    soundManager = new SoundManager();
    scriptManager = new ScriptManager();
    debugDrawingManager = new DebugDrawingManager(renderer);
    profilingManager = new ProfilingManager();
    triggerManager = new TriggerManager();
}

void Hub::ShutDown() {
    delete triggerManager;
    delete profilingManager;
    delete debugDrawingManager;
    delete scriptManager;
    delete soundManager;
    delete renderManager;
    delete physicsManager;
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
