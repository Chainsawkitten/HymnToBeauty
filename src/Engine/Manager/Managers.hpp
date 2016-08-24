#pragma once

class ResourceManager;
class RenderManager;
class PhysicsManager;
class SoundManager;

/// Singleton class that holds all subsystems.
class Hub {
    friend Hub& Managers();
    
    public:
        /// The resource manager instance.
        ResourceManager* resourceManager;
        
        /// The render manager instance.
        RenderManager* renderManager;
        
        /// The physics manager instance.
        PhysicsManager* physicsManager;
        
        /// The sound manager instance.
        SoundManager* soundManager;
        
        /// Initialize all subsystems.
        void StartUp();
        
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
