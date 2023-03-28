#include "ProfilingWindow.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <Utility/Log.hpp>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cstring>

using namespace GUI;

void ProfilingWindow::Show() {
    // Show the results.
    ImGui::Begin("Profiling", &visible);

    // Frametimes.
    unsigned int frames = Managers().profilingManager->GetFrameCount();
    if (ImGui::CollapsingHeader("Frametimes")) {
        if (ImGui::TreeNode("CPU Frametimes")) {
            ImGui::PlotLines("Frametimes in ms", Managers().profilingManager->GetCPUFrameTimes(), frames, 0, nullptr, 0.f, FLT_MAX, ImVec2(0.f, 300.f));
            ImGui::TreePop();
        }
    }

    // Timeline.
    if (ImGui::CollapsingHeader("Timeline")) {
        ShowTimeline();

        // Timeline statistics.
        if (ImGui::TreeNode("Statistics")) {
            ShowStatistics();
            ImGui::TreePop();
        }
    }

    // Show the log.
    if (ImGui::CollapsingHeader("Log"))
        logView.Show();

    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

bool ProfilingWindow::IsVisible() const {
    return visible;
}

void ProfilingWindow::SetVisible(bool visible) {
    this->visible = visible;
}

void ProfilingWindow::LoadLatest() {
    std::string filename = "";

    // Find latest profiling timeline.
    std::vector<std::string> timelines = FileSystem::DirectoryContents(FileSystem::DataPath("Hymn to Beauty", "Profiling"), FileSystem::FILE);
    for (const std::string& file : timelines) {
        if (FileSystem::GetExtension(file) == "json") {
            if (filename.empty() || strcmp(file.c_str(), filename.c_str()) > 0)
                filename = file;
        }
    }

    // Load timeline.
    if (!filename.empty())
        FileSelected(FileSystem::DataPath("Hymn to Beauty", "Profiling/") + filename);
}

void ProfilingWindow::FileSelected(const std::string& filename) {
    Json::Value root;
    std::ifstream file(filename);
    file >> root;
    file.close();

    timeline.FromJson(root);
    ParseTimeline();
    CalculateStatistics();
}

void ProfilingWindow::ParseTimeline() {
    threadViews.clear();
    earliestTime = std::numeric_limits<double>::max();

    for (const Profiling::Thread& thread : timeline.threads) {
        ThreadView threadView;
        threadView.name = thread.name;
        threadView.lines = 0;

        for (const Profiling::Event& event : thread.events) {
            ParseEvent(threadView, event, 0);
        }

        threadViews.push_back(threadView);
    }

    scale = 100.0f;
    resetScroll = true;
}

void ProfilingWindow::ParseEvent(ThreadView& threadView, const Profiling::Event& event, unsigned int line) {
    // Add line if we've reached a new depth.
    if (line >= threadView.lines) {
        threadView.events.push_back(std::vector<Profiling::Event>());
        threadView.lines++;
    }

    // Add this event.
    threadView.events[line].push_back(event);
    if (event.time < earliestTime)
        earliestTime = event.time;

    // Parse children.
    for (const Profiling::Event& child : event.children) {
        ParseEvent(threadView, child, line + 1);
    }
}

void ProfilingWindow::ShowTimeline() {
    if (ImGui::Button("Load latest")) {
        LoadLatest();
    }

    ImGui::SameLine();

    if (ImGui::Button("Load JSON")) {
        fileSelector.AddExtensions("json");
        fileSelector.SetInitialPath(FileSystem::DataPath("Hymn to Beauty", "Profiling").c_str());
        fileSelector.SetFileSelectedCallback(std::bind(&ProfilingWindow::FileSelected, this, std::placeholders::_1));
        fileSelector.SetVisible(true);
    }

    // Scale slider.
    ImGui::SameLine();
    const float oldScale = scale;
    const float minScale = 100.0f;
    const float maxScale = 10000.0f;
    ImGui::SliderFloat("Zoom", &scale, minScale, maxScale, "%.0f%%");
    pixelsPerSecond = 100.0f * oldScale;

    // Mouse zoom.
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && io.MouseWheel != 0.0f) {
        scale += io.MouseWheel * 100.0f;
        scale = std::min(std::max(scale, minScale), maxScale);
    }

    unsigned int totalLines = 0;
    for (const ThreadView& threadView : threadViews) {
        totalLines += threadView.lines;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    // Thread names.
    ImGui::BeginChild("Thread names", ImVec2(100, totalLines *  ImGui::GetFrameHeightWithSpacing() + 30.0f + rulerHeight), false);
    ShowThreadNames();
    ImGui::EndChild();

    // Timeline.
    ImGui::SameLine();
    ImGui::BeginChild("Timeline", ImVec2(0, totalLines *  ImGui::GetFrameHeightWithSpacing() + 30.0f + rulerHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (scale != oldScale) {
        // Adjust scroll position so it remains at the same time.
        ImGui::SetScrollX(ImGui::GetScrollX() / oldScale * scale);
    }

    if (resetScroll) {
        ImGui::SetScrollX(0.0f);
        resetScroll = false;
    }

    // Mouse dragging.
    ImGuiContext& context = *ImGui::GetCurrentContext();
    if (context.HoveredId == 0) {
        bool hovered = false;
        bool held = false;

        ImGui::ButtonBehavior(context.CurrentWindow->Rect(), ImGui::GetID("##scrolldraggingoverlay"), &hovered, &held, ImGuiMouseButton_Left);
        const ImVec2 delta = io.MouseDelta;
        if (held && delta.x != 0.0f)
            ImGui::SetScrollX(ImGui::GetScrollX() - delta.x);
    }

    ShowRuler();

    // Timeline events.
    ShowThreads();

    ImGui::PopStyleVar(3);

    ImGui::EndChild();
}

void ProfilingWindow::ShowRuler() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    const double beginTime = ImGui::GetScrollX() / pixelsPerSecond;
    const double endTime = (ImGui::GetScrollX() + ImGui::GetWindowWidth()) / pixelsPerSecond;
    const float minPixelsPerRule = 100.0f;
    const double step1 = minPixelsPerRule / pixelsPerSecond;
    double step = pow(10.0, ceil(log10(step1)));
    step = (step1 < step * 0.5) ? step * 0.5 : step;
    step = (step1 < step * 0.5) ? step * 0.5 : step;

    for (double time = beginTime - fmod(beginTime, step); time < endTime; time += step) {
        ImVec2 position = ImVec2(cursorPos.x + time * pixelsPerSecond, cursorPos.y);
        drawList->AddRectFilled(position, ImVec2(position.x + 1.0f, position.y + rulerHeight), color);
        if (position.x - cursorPos.x + minPixelsPerRule < ImGui::GetScrollX() + ImGui::GetWindowWidth()) {
            ImGui::SetCursorScreenPos(ImVec2(position.x + 2.0f, position.y));
            ImGui::Text("%.2f ms", time * 1000.0);
        }
    }

    for (double time = beginTime - fmod(beginTime, step); time < endTime; time += step * 0.1) {
        ImVec2 position = ImVec2(cursorPos.x + time * pixelsPerSecond, cursorPos.y);
        drawList->AddRectFilled(ImVec2(position.x, position.y + rulerHeight * 0.75f), ImVec2(position.x + 1.0f, position.y + rulerHeight), color);
    }
}

void ProfilingWindow::ShowThreadNames() {
    ImGui::SetCursorPosY(rulerHeight);
    ImGui::Separator();

    unsigned int lines = 0;

    for (const ThreadView& threadView : threadViews) {
        ImGui::SetCursorPosY(rulerHeight + lines * ImGui::GetFrameHeightWithSpacing());
        ImGui::Text(threadView.name.c_str());

        lines += threadView.lines;
        ImGui::SetCursorPosY(rulerHeight + lines * ImGui::GetFrameHeightWithSpacing());

        ImGui::Separator();
    }
}

void ProfilingWindow::ShowThreads() {
    ImGui::SetCursorPosY(rulerHeight);
    ImGui::Separator();

    unsigned int lines = 0;

    for (const ThreadView& threadView : threadViews) {
        for (unsigned int line = 0; line < threadView.lines; line++) {
            for (std::size_t i = 0; i < threadView.events[line].size(); i++) {
                if (i > 0)
                    ImGui::SameLine();

                ImGui::PushID(i + line * 10000);

                const Profiling::Event& event = threadView.events[line][i];
                ImGui::SetCursorPosX(static_cast<float>((event.time - earliestTime) * pixelsPerSecond));
                ImGui::SetCursorPosY(rulerHeight + (lines + line) * ImGui::GetFrameHeightWithSpacing());
                ImGui::Button(event.name.c_str(), ImVec2(std::max(static_cast<float>(event.duration * pixelsPerSecond), 1.0f), 0.0f));

                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s - %f ms", event.name.c_str(), event.duration * 1000.0f);
                    ImGui::EndTooltip();
                }

                ImGui::PopID();
            }
        }

        lines += threadView.lines;
        ImGui::SetCursorPosY(rulerHeight + lines * ImGui::GetFrameHeightWithSpacing());

        ImGui::Separator();
    }
}

void ProfilingWindow::CalculateStatistics() {
    statistics.clear();
    statistics.resize(threadViews.size());
    frameCount = 0;

    for (std::size_t i = 0; i < threadViews.size(); ++i) {
        // Parse events from timeline.
        std::map<std::string, EventInfo> workingStats;
        for (unsigned int line = 0; line < threadViews[i].lines; ++line) {
            for (const Profiling::Event& event : threadViews[i].events[line]) {
                std::string name = event.name;
                if (i == 0 && line == 0) {
                    ++frameCount;
                    name = "Frame";
                }

                EventInfo& eventInfo = workingStats[name];
                eventInfo.count++;
                eventInfo.totalDuration += event.duration;
            }
        }

        // Convert to vector.
        for (auto& it : workingStats) {
            EventInfo eventInfo = it.second;
            eventInfo.name = it.first;

            statistics[i].push_back(eventInfo);
        }
    }
}

void ProfilingWindow::ShowStatistics() {
    for (std::size_t i = 0; i < threadViews.size(); ++i) {
        ImGui::Text("%s", threadViews[i].name.c_str());

        ImGui::Indent();

        if (ImGui::BeginTable(threadViews[i].name.c_str(), 3, ImGuiTableFlags_Sortable)) {
            ImGui::TableSetupColumn("Event");
            ImGui::TableSetupColumn("Count (/frame)");
            ImGui::TableSetupColumn("Duration (ms / frame)");
            ImGui::TableHeadersRow();

            ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
            if (sortSpecs != nullptr && sortSpecs->SpecsDirty) {
                std::sort(statistics[i].begin(), statistics[i].end(), [sortSpecs](const EventInfo& a, const EventInfo& b) -> bool {
                    switch (sortSpecs->Specs[0].ColumnIndex) {
                    case 0:
                        if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending) {
                            return a.name < b.name;
                        } else {
                            return b.name < a.name;
                        }
                        break;
                    case 1:
                        if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending) {
                            return a.count < b.count;
                        } else {
                            return b.count < a.count;
                        }
                        break;
                    case 2:
                        if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending) {
                            return a.totalDuration < b.totalDuration;
                        } else {
                            return b.totalDuration < a.totalDuration;
                        }
                        break;
                    }
                    });
            }

            for (const EventInfo& eventInfo : statistics[i]) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", eventInfo.name.c_str());

                ImGui::TableNextColumn();
                if (eventInfo.count % frameCount == 0) {
                    ImGui::Text("%i", eventInfo.count / frameCount);
                } else {
                    ImGui::Text("%f", static_cast<float>(eventInfo.count) / frameCount);
                }

                ImGui::TableNextColumn();
                ImGui::Text("%f", eventInfo.totalDuration * 1000.0 / static_cast<double>(frameCount));
            }

            ImGui::EndTable();
        }

        ImGui::Unindent();
    }
}
