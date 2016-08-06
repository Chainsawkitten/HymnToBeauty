#include "MeshEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Geometry/OBJModel.hpp>
#include "../../TextButton.hpp"
#include "../../ModelSelector.hpp"

using namespace GUI;

MeshEditor::MeshEditor(Widget* parent, ModelSelector* modelSelector) : ComponentEditor(parent, "Mesh") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    selectModelButton = new TextButton(this, font, "Select model");
    selectModelButton->SetClickedCallback(std::bind(&SelectModelPressed, this));
    AddEditor("Model", selectModelButton);
    
    this->modelSelector = modelSelector;
    
    SetVisible(false);
}

MeshEditor::~MeshEditor() {
    Managers().resourceManager->FreeFont(font);
}

void MeshEditor::SetEntity(Entity* entity) {
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        mesh = entity->GetComponent<Component::Mesh>();
        SetVisible(mesh != nullptr);
    }
}

void MeshEditor::SelectModelPressed() {
    modelSelector->SetVisible(true);
    modelSelector->UpdateModels();
    modelSelector->SetModelSelectedCallback(std::bind(&ModelSelected, this, std::placeholders::_1));
}

void MeshEditor::ModelSelected(Geometry::OBJModel* model) {
    mesh->geometry = model;
}
