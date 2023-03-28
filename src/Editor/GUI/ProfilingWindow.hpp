#pragma once

#include <Engine/Manager/ProfilingManager.hpp>
#include <GUI/LogView.hpp>
#include <map>
#include "FileSelector.hpp"

namespace GUI {
class ProfilingWindow {
  public:
    /// Show profiling results.
    void Show();

    /// Get whether the window is visible.
    /**
     * @return Whether the window is visible.
     */
    bool IsVisible() const;

    /// Set whether the window should be visible.
    /**
     * @param visible Whether the window should be visible.
     */
    void SetVisible(bool visible);

  private:
    struct ThreadView {
        std::string name;
        unsigned int lines;
        std::vector<std::vector<Profiling::Event>> events;
    };

    LogView logView;

    void LoadLatest();
    void FileSelected(const std::string& filename);

    void ParseTimeline();
    void ParseEvent(ThreadView& threadView, const Profiling::Event& event, unsigned int line);

    void ShowTimeline();
    void ShowRuler();
    void ShowThreadNames();
    void ShowThreads();

    void CalculateStatistics();
    void ShowStatistics();

    bool visible = false;

    FileSelector fileSelector;

    Profiling::Timeline timeline;
    double earliestTime;
    float scale = 100.0f;
    double pixelsPerSecond;
    const float rulerHeight = 24.0f;
    bool resetScroll = false;

    std::vector<ThreadView> threadViews;

    struct EventInfo {
        double totalDuration = 0.0;
        uint32_t count = 0;
    };
    std::vector<std::map<std::string, EventInfo>> statistics;
};
} // namespace GUI
