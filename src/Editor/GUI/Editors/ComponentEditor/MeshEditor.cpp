#include "MeshEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Mesh.hpp>
#include "../../TextButton.hpp"
#include <Engine/Util/Log.hpp>

using namespace GUI;

MeshEditor::MeshEditor(Widget* parent) : ComponentEditor(parent, "Mesh") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    selectModelButton = new TextButton(this, font, "Select model");
    AddEditor("Model", selectModelButton);
    
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
    Log() << "Select model pressed.";
}
