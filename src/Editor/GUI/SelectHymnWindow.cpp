#include "SelectHymnWindow.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;
using namespace std;

void SelectHymnWindow::Scan() {
    files = FileSystem::DirectoryContents(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns", FileSystem::DIRECTORY);
}

void SelectHymnWindow::Show() {
    ImGui::OpenPopup(title);
    
    // Create new hymn.
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders)) {
        for (string file : files) {
            if (ImGui::Button(file.c_str()))
                strcpy(name, file.c_str());
        }
        
        ImGui::InputText("Name", name, 128);
        
        if (ImGui::Button(openButtonName, ImVec2(120,0))) {
            closedCallback(name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120,0))) {
            closedCallback("");
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SelectHymnWindow::SetClosedCallback(std::function<void(const std::string&)> callback) {
    closedCallback = callback;
    hasClosedCallback = true;
}

bool SelectHymnWindow::IsVisible() const {
    return visible;
}

void SelectHymnWindow::SetVisible(bool visible) {
    this->visible = visible;
}

void SelectHymnWindow::SetTitle(const char* title) {
    this->title = title;
}

void SelectHymnWindow::SetOpenButtonName(const char* openButtonName) {
    this->openButtonName = openButtonName;
}
