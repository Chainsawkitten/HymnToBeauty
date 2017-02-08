#include "SceneEditor.hpp"

#include <imgui.h>

using namespace GUI;

void SceneEditor::Show() {
    if (ImGui::Begin(("Scene: " + *scene + "###Scene").c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        *scene = name;
    }
    ImGui::End();
}

void SceneEditor::SetScene(std::string* scene) {
    this->scene = scene;
    
    strcpy(name, scene->c_str());
}

bool SceneEditor::IsVisible() const {
    return visible;
}

void SceneEditor::SetVisible(bool visible) {
    this->visible = visible;
}
