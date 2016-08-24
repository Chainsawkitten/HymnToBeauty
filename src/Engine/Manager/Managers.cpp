#include "Managers.hpp"

#include "ResourceManager.hpp"
#include "RenderManager.hpp"
#include "PhysicsManager.hpp"
#include "SoundManager.hpp"

Hub::Hub() {
    
}

Hub& Managers() {
    static Hub instance;
    
    return instance;
}

void Hub::StartUp() {
    resourceManager = new ResourceManager();
    renderManager = new RenderManager();
    physicsManager = new PhysicsManager();
    soundManager = new SoundManager();
}

void Hub::ShutDown() {
    delete soundManager;
    delete renderManager;
    delete physicsManager;
    delete resourceManager;
}
