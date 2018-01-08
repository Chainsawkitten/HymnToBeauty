#include "SelectHymnWindow.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;
using namespace std;

SelectHymnWindow::SelectHymnWindow(): title(nullptr), openButtonName(nullptr) {
    name[0] = '\0';
}

void SelectHymnWindow::Scan() {
    files = FileSystem::DirectoryContents(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns", FileSystem::DIRECTORY);
}

void SelectHymnWindow::Show() {
    ImGui::OpenPopup(title);
    
    // Create new hymn.
    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        for (string file : files) {
            if (ImGui::Button(file.c_str()))
                strcpy(name, file.c_str());
        }
        
        ImGui::InputText("Name", name, 128);
        
        if (ImGui::Button(openButtonName, ImVec2(120, 0))) {
            closedCallback(name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            closedCallback("");
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SelectHymnWindow::SetClosedCallback(const std::function<void(const std::string&)>& callback) {
    closedCallback = callback;
    hasClosedCallback = true;
}

bool SelectHymnWindow::IsVisible() const {
    return visible;
}

void SelectHymnWindow::SetVisible(bool visible) {
    this->visible = visible;
    name[0] = '\0';
}

void SelectHymnWindow::SetTitle(const char* title) {
    this->title = title;
}

void SelectHymnWindow::SetOpenButtonName(const char* openButtonName) {
    this->openButtonName = openButtonName;
}
