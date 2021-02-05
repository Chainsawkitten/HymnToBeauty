#include "SavePromptWindow.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include "../ImGui/GuiHelpers.hpp"

using namespace GUI;
using namespace std;

void SavePromptWindow::Show() {
    ImGui::SetNextWindowPosCenter();
    ImGui::OpenPopup(title.c_str());

    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            decision = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Don't save", ImVec2(120, 0))) {
            decision = 1;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            decision = 2;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

int SavePromptWindow::GetDecision() const {
    return decision;
}

void SavePromptWindow::ResetDecision() {
    decision = -1;
}

void SavePromptWindow::SetTitle(const std::string& newTitle) {
    title = newTitle;
}

bool SavePromptWindow::IsVisible() const {
    return visible;
}

void SavePromptWindow::SetVisible(bool visible) {
    this->visible = visible;
}

void GUI::SavePromptWindow::SetDecision(int option) {
    if (option == 0) {
        decision = 0;
    }

    if (option == 1) {
        decision = 1;
    }

    if (option == 2) {
        decision = 2;
    }
}
