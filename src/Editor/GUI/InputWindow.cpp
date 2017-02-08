#include "InputWindow.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <Engine/Input/Input.hpp>

#include <imgui.h>

using namespace GUI;
using namespace std;

void InputWindow::Show() {

    ImGui::OpenPopup("Input");
    
    // Create new hymn.
    if (ImGui::BeginPopupModal("Input", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

		if (ImGui::Button("Add Button")) {

			AddButton();

		}

		for (Input::Button button : Input::GetInstance().buttons) {

			char temp_action[255];
			strcpy(temp_action, button.action.c_str());
			if (ImGui::InputText(button.action.c_str(), temp_action, 255)) {

				button.action = temp_action;

			}
			ImGui::SameLine();
			ImGui::InputInt("Key", &(button.key));
			ImGui::SameLine();
			ImGui::InputInt("State", &(button.state));

		}

		ImGui::EndPopup();

    }
}

void InputWindow::AddButton() {

	Input::Button button;
	button.action = "action";
	button.key = 0;
	button.state = 0;

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