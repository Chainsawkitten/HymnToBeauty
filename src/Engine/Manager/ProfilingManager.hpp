#pragma once

#include <string>
#include <list>
#include <map>

#include <Utility/Profiling/Timeline.hpp>

namespace Video {
class LowLevelRenderer;
}

/// Handles profiling.
class ProfilingManager {
    friend class Hub;
    friend class ProfilingScope;

  public:
    /// Begin profiling a frame.
    void BeginFrame();

    /// End profiling a frame and collect the results.
    void EndFrame();

    /// Check whether %ProfilingManager is active.
    /**
     * @return Active state.
     */
    bool Active() const;

    /// Set whether %ProfilingManager is active.
    /**
     * @param active Active state.
     */
    void SetActive(bool active);

    /// Get number of frames being monitored.
    /**
     * @return The number of frames being stored.
     */
    unsigned int GetFrameCount() const;

    /// Get the measured CPU frame times.
    /**
     * @return The CPU frame times.
     */
    const float* GetCPUFrameTimes() const;

    /// Get GPU timeline from the render manager.
    void FetchGPUTimeline();

    /// Get the timeline.
    /**
     * @return The timeline.
     */
    const Profiling::Timeline& GetTimeline() const;

  private:
    ProfilingManager();
    ~ProfilingManager();
    ProfilingManager(ProfilingManager const&) = delete;
    void operator=(ProfilingManager const&) = delete;

    Profiling::Event* StartEvent(const std::string& name);
    void FinishEvent(Profiling::Event* event);

    bool active;

    Profiling::Timeline timeline;
    Profiling::Thread* mainThread;
    Profiling::Event* currentEvent;
    Profiling::Event* currentFrameEvent;

    Video::LowLevelRenderer* lowLevelRenderer;
    Profiling::Thread* gpuThread;

    unsigned int currentFrame = 0;

    double frameStart;
    static const unsigned int frames = 100;
    unsigned int frame = 0;
    float frameTimes[2][frames];
};
