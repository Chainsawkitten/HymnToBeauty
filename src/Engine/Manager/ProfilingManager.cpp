#include "ProfilingManager.hpp"

#include <GLFW/glfw3.h>
#include <Utility/Log.hpp>
#include "RenderManager.hpp"
#include "Managers.hpp"
#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <fstream>
#include <Engine/Util/FileSystem.hpp>
#include <chrono>

ProfilingManager::ProfilingManager() : active(false) {
    // Add threads to the timeline.
    mainThread = timeline.AddThread("Main thread");
    gpuThread = timeline.AddThread("GPU");

    lowLevelRenderer = Managers().renderManager->GetRenderer()->GetLowLevelRenderer();
}

ProfilingManager::~ProfilingManager() {
    
}

void ProfilingManager::BeginFrame() {
    frameStart = glfwGetTime();

    if (active) {
        // Add new frame node to main thread.
        currentFrameEvent = mainThread->AddEvent("Frame " + std::to_string(currentFrame));
        currentFrameEvent->time = glfwGetTime();
        currentFrameEvent->duration = 0.0;
        currentEvent = currentFrameEvent;
    }
}

void ProfilingManager::EndFrame() {
    // Calculate the CPU time of this frame.
    frameTimes[0][frame] = static_cast<float>((glfwGetTime() - frameStart) * 1000.0);

    if (++frame >= frames)
        frame = 0;

    if (active) {
        // Get main thread time of the frame.
        currentFrameEvent->duration = glfwGetTime() - currentFrameEvent->time;
    }

    currentFrame++;
}

bool ProfilingManager::Active() const {
    return active;
}

void ProfilingManager::SetActive(bool active) {
    this->active = active;
    lowLevelRenderer->SetProfiling(active);

    if (active) {
        // Clear previous results when beginning profiling.
        for (Profiling::Thread& thread : timeline.threads) {
            thread.events.clear();
        }

        Log(Log::INFO) << "Profiling started.\n";
    } else {
        FileSystem::CreateDirectory(FileSystem::DataPath("Hymn to Beauty", "Profiling").c_str());

        // Save timeline to file.
        time_t today = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm* timeinfo = localtime(&today);

        const unsigned int bufferLength = 24;

        char buffer[bufferLength] = {'\0'};
        strftime(buffer, bufferLength, "%Y-%m-%d %H-%M-%S", timeinfo);
        const std::string dateString = std::string(buffer);

        std::string path = FileSystem::DataPath("Hymn to Beauty", ("Profiling/" + dateString + ".json").c_str());
        std::ofstream file(path);
        file << timeline.ToJson();
        file.close();

        Log(Log::INFO) << "Profiling results written to " << path << "\n";
    }
}

unsigned int ProfilingManager::GetFrameCount() const {
    return frames;
}

const float* ProfilingManager::GetCPUFrameTimes() const {
    return frameTimes[0];
}

void ProfilingManager::FetchGPUTimeline() {
    const std::vector<Profiling::Event>& gpuEvents = lowLevelRenderer->GetTimeline();

    for (const Profiling::Event& event : gpuEvents) {
        gpuThread->events.push_back(event);
    }
}

const Profiling::Timeline& ProfilingManager::GetTimeline() const {
    return timeline;
}

Profiling::Event* ProfilingManager::StartEvent(const std::string& name) {
    assert(active);

    currentEvent->children.push_back(Profiling::Event(name));
    Profiling::Event* event = &currentEvent->children.back();
    event->parent = currentEvent;
    event->time = glfwGetTime();
    currentEvent = event;

    return event;
}

void ProfilingManager::FinishEvent(Profiling::Event* event) {
    assert(active);
    assert(event != nullptr);

    currentEvent->duration = glfwGetTime() - currentEvent->time;
    currentEvent = currentEvent->parent;
}
