#include "FolderNameWindow.hpp"

#include <imgui.h>

using namespace GUI;

void FolderNameWindow::Show() {
    const char* title = "Enter folder name";
    ImGui::OpenPopup(title);

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Name", name, 128);

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            visible = false;
            closedCallback(name);
            name[0] = '\0';
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            name[0] = '\0';
            visible = false;
            closedCallback(name);
        }
        ImGui::EndPopup();
    }
}

void FolderNameWindow::SetVisible(bool visible) {
    this->visible = visible;
}

bool FolderNameWindow::IsVisible() const {
    return visible;
}

void FolderNameWindow::SetClosedCallback(const std::function<void(const std::string&)>& callback) {
    closedCallback = callback;
}
