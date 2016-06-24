#include "Managers.hpp"

#include "ResourceManager.hpp"

Hub::Hub() {
    
}

Hub& Managers() {
    static Hub instance;
    
    return instance;
}

void Hub::StartUp() {
    resourceManager = new ResourceManager();
}

void Hub::ShutDown() {
    delete resourceManager;
}
