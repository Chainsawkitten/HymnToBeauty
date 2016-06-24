#include "Managers.hpp"

#include "ResourceManager.hpp"
#include "RenderManager.hpp"

Hub::Hub() {
    
}

Hub& Managers() {
    static Hub instance;
    
    return instance;
}

void Hub::StartUp() {
    resourceManager = new ResourceManager();
    renderManager = new RenderManager();
}

void Hub::ShutDown() {
    delete renderManager;
    delete resourceManager;
}
