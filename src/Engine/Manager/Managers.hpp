#pragma once

#include <Video/Renderer.hpp>

class ResourceManager;
class RenderManager;
class PhysicsManager;
class SoundManager;
class ScriptManager;
class DebugDrawingManager;
class ProfilingManager;
class TriggerManager;

/// Singleton class that holds all subsystems.
class Hub {
    friend Hub& Managers();

  public:
    // The renderer used to render things.
    Video::Renderer* renderer;

    /// The resource manager instance.
    ResourceManager* resourceManager;

    /// The render manager instance.
    RenderManager* renderManager;

    /// The physics manager instance.
    PhysicsManager* physicsManager;

    /// The sound manager instance.
    SoundManager* soundManager;

    /// The script manager instance.
    ScriptManager* scriptManager;

    /// The debug drawing manager instance.
    DebugDrawingManager* debugDrawingManager;

    /// The profiling manager instance.
    ProfilingManager* profilingManager;

    /// The trigger manager instance.
    TriggerManager* triggerManager;

    /// Initialize all subsystems.
    /**
     * @param graphicsAPI Which graphics API to use for rendering.
     */
    void StartUp(Video::Renderer::GraphicsAPI graphicsAPI);

    /// Deinitialize all subsystems.
    void ShutDown();

    /// Clears the killed components of all managers.
    void ClearKilledComponents();

  private:
    Hub();
    Hub(const Hub&) = delete;
    void operator=(const Hub&) = delete;

    bool shutdown = false;
};

/// Get a hub containing all the subsystems.
/**
 * @return A hub containing all the subsystems.
 */
Hub& Managers();
