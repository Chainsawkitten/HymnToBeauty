#pragma once

class ResourceManager;
class RenderManager;
class ParticleManager;
class PhysicsManager;
class SoundManager;
class DebugDrawingManager;

/// Singleton class that holds all subsystems.
class Hub {
    friend Hub& Managers();
    
    public:
        /// The resource manager instance.
        ResourceManager* resourceManager;
        
        /// The render manager instance.
        RenderManager* renderManager;
        
        /// The particle manager instance.
        ParticleManager* particleManager;
        
        /// The physics manager instance.
        PhysicsManager* physicsManager;
        
        /// The sound manager instance.
        SoundManager* soundManager;
        
        /// The debug drawing manager instance.
        DebugDrawingManager* debugDrawingManager;
        
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
