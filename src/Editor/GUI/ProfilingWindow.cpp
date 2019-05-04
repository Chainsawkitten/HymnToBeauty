#include "ProfilingWindow.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <imgui.h>
#include <Utility/Log.hpp>

using namespace GUI;

void ProfilingWindow::Show() {
    if (!Managers().profilingManager->Active()) {
        Log() << "ProfilingManager::ShowResults warning: Not active.\n";
        return;
    }

    // Show the results.
    ImGui::Begin("Profiling", nullptr);

    // Frametimes.
    unsigned int frames = Managers().profilingManager->GetFrameCount();
    if (ImGui::CollapsingHeader("Frametimes")) {
        if (ImGui::TreeNode("CPU Frametimes")) {
            ImGui::PlotLines("Frametimes in ms", Managers().profilingManager->GetCPUFrameTimes(), frames, 0, nullptr, 0.f, FLT_MAX, ImVec2(0.f, 300.f));
            ImGui::TreePop();
        }
    }

    // Breakdown.
    if (ImGui::CollapsingHeader("Breakdown")) {
        for (int i = 0; i < ProfilingManager::COUNT; ++i) {
            ProfilingManager::Result* result = Managers().profilingManager->GetResult((ProfilingManager::Type)i);
            int flags = result->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
            if (ImGui::TreeNodeEx(ProfilingManager::TypeToString((ProfilingManager::Type)i).c_str(), flags)) {
                ImGui::Columns(2);
                for (ProfilingManager::Result& child : result->children)
                    ShowResult(&child);
                ImGui::Columns(1);
                ImGui::TreePop();
            }
        }

        ImGui::Text("Light count: %u", Managers().renderManager->GetLightCount());
    }

    /// Update log whether or not we're actually showing it.
    logView.UpdateLog();
    if (ImGui::CollapsingHeader("Log"))
        logView.Show();

    ImGui::End();
}

void ProfilingWindow::ShowResult(ProfilingManager::Result* result) {
    ImGui::AlignFirstTextHeightToWidgets();
    int flags = result->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
    bool expanded = ImGui::TreeNodeEx(result->name.c_str(), flags);

    ImGui::NextColumn();
    if (result->parent->parent != nullptr) {
        ImGui::ProgressBar(static_cast<float>(result->value / result->parent->value), ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    }

    ImGui::Text(std::to_string(result->value).c_str());
    ImGui::NextColumn();

    if (expanded) {
        double otherTime = result->value;
        for (ProfilingManager::Result& child : result->children) {
            ShowResult(&child);
            otherTime -= child.value;
        }

        if (!result->children.empty()) {
            ProfilingManager::Result other("Other", result);
            other.value = otherTime;
            ShowResult(&other);
        }

        ImGui::TreePop();
    }
}
