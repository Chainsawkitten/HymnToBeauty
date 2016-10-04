#include "ModelEditor.hpp"

#include <Engine/Geometry/OBJModel.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;

void ModelEditor::Show() {
    if (ImGui::Begin(("Model: " + model->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(model))).c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        model->name = name;
        
        if (ImGui::Button("Load OBJ model")) {
            fileSelector.SetExtension("obj");
            fileSelector.SetFileSelectedCallback(std::bind(&ModelEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
    }
    ImGui::End();
    
    if (fileSelector.IsVisible())
        fileSelector.Show();
}

void ModelEditor::SetModel(Geometry::OBJModel* model) {
    this->model = model;
    
    strcpy(name, model->name.c_str());
}

bool ModelEditor::IsVisible() const {
    return visible;
}

void ModelEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void ModelEditor::FileSelected(const std::string& file) {
    std::string destination = Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + model->name + ".obj";
    FileSystem::Copy(file.c_str(), destination.c_str());
    model->Load(file.c_str());
}
