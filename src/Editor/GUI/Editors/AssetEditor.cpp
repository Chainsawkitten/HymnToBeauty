#include "AssetEditor.hpp"

#include <Engine/Geometry/Model.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include <Utility/Log.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Material.hpp>

using namespace GUI;

AssetEditor::AssetEditor() {
    name[0] = '\0';
}

void AssetEditor::Show() {
    if (ImGui::Begin(("Model: " + model->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(model))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            model->name = name;

            // Rename the file.
            if (FileSystem::FileExists((destination + ".asset").c_str())) {
                std::string newDestination = Hymn().GetPath() + "/" + model->path + name;

                rename((destination + ".asset").c_str(), (newDestination + ".asset").c_str());
                rename((destination + ".asset.meta").c_str(), (newDestination + ".asset.meta").c_str());

                destination = newDestination;
            }
        }
    }
    ImGui::End();
}

const Geometry::Model* AssetEditor::GetModel() const {
    return model;
}

void AssetEditor::SetModel(Geometry::Model* model) {
    this->model = model;

    strcpy(name, model->name.c_str());

    destination = Hymn().GetPath() + "/" + model->path + name;
}

bool AssetEditor::IsVisible() const {
    return visible;
}

void AssetEditor::SetVisible(bool visible) {
    this->visible = visible;
}
