#include "InputWindow.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <Engine/MainWindow.hpp>
#include <Engine/Input/Input.hpp>

#include <imgui.h>

using namespace GUI;
using namespace std;

void InputWindow::Show() {

    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);

    ImGui::SetNextWindowPosCenter();
    ImGui::OpenPopup("Input");
    
    // Create new hymn.
    if (ImGui::BeginPopupModal("Input", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        if (ImGui::Button("Add Button")) {
            AddButton();
        }
        if (ImGui::Button("Close")) {
            SetVisible(false);
            ImGui::CloseCurrentPopup();
        }
        unsigned int button_number = 0;
        for (Input::Button* button : Input::GetInstance().buttons) {
            ImGui::Text("Action");
            ImGui::SameLine();
            ImGui::InputText(("##action" + std::to_string(button_number)).c_str(), button->action, 255);
            ImGui::SameLine();
            ImGui::Text("Key");
            ImGui::SameLine();
            ImGui::InputInt(("##key" + std::to_string(button_number)).c_str(), &(button->key));
            ImGui::SameLine();
            ImGui::Text("State");
            ImGui::SameLine();
            ImGui::InputInt(("##state" + std::to_string(button_number)).c_str(), &(button->state));

            button_number++;

        }

        ImGui::EndPopup();

    }
}

void InputWindow::AddButton() {

    Input::Button* button = new Input::Button();
    strcpy(button->action, "action");
    button->key = 0;
    button->state = 0;

    Input::GetInstance().buttons.push_back(button);

}

void InputWindow::SetClosedCallback(std::function<void(const std::string&)> callback) {
    closedCallback = callback;
    hasClosedCallback = true;
}

bool InputWindow::IsVisible() const {
    return visible;
}

void InputWindow::SetVisible(bool visible) {
    this->visible = visible;
}