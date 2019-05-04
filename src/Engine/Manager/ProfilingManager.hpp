#pragma once

#include <string>
#include <list>
#include <map>

/// Handles profiling.
class ProfilingManager {
    friend class Hub;
    friend class Profiling;
    friend class GPUProfiling;

  public:
    /// The type of profiling to perform.
    enum Type { CPU_TIME = 0, COUNT };

    /// A profiling result.
    struct Result {
        std::string name;
        double value = 0.0;
        std::list<Result> children;
        Result* parent;

        Result(const std::string& name, Result* parent);
    };

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

    /// Get profiling result.
    /**
     * @param type The type of profiling to get results for.
     * @return The measured result.
     */
    Result* GetResult(Type type) const;

    /// Get the name of a type of profiling.
    /**
     * @param type The type of profiling.
     * @return The name.
     */
    static std::string TypeToString(Type type);

  private:
    ProfilingManager();
    ~ProfilingManager();
    ProfilingManager(ProfilingManager const&) = delete;
    void operator=(ProfilingManager const&) = delete;

    Result* StartResult(const std::string& name, Type type);
    void FinishResult(Result* result, Type type);

    bool active;

    Result* root[Type::COUNT];
    Result* current[Type::COUNT];

    double frameStart;
    static const unsigned int frames = 100;
    unsigned int frame = 0;
    float frameTimes[2][frames];
};
