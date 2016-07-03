#pragma once

#include <glm/glm.hpp>

class ResourceManager;
class RenderManager;

/// Singleton class that holds all subsystems.
class Hub {
    friend Hub& Managers();
    
    public:
        /// The resource manager instance.
        ResourceManager* resourceManager;
        
        /// The render manager instance.
        RenderManager* renderManager;
        
        /// Initialize all subsystems.
        /**
         * @param screenSize The size of the screen in pixels.
         */
        void StartUp(const glm::vec2& screenSize);
        
        /// Deinitialize all subsystems.
        void ShutDown();
        
    private:
        Hub();
        Hub(const Hub&) = delete;
        void operator=(const Hub&) = delete;
};

/// Get a hub containing all the subsystems.
/**
 * @return A hub containing all the subsystems.
 */
Hub& Managers();
