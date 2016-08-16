#include "LensEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Lens.hpp>
#include "../FloatEditor.hpp"

using namespace GUI;

LensEditor::LensEditor(Widget* parent) : ComponentEditor(parent, "Lens") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::Lens>();
    
    fieldOfViewEditor = new FloatEditor(this, font);
    AddEditor("Field of view", fieldOfViewEditor);
    
    zNearEditor = new FloatEditor(this, font);
    AddEditor("Z near", zNearEditor);
    
    zFarEditor = new FloatEditor(this, font);
    AddEditor("Z far", zFarEditor);
    
    SetVisible(false);
}

LensEditor::~LensEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete fieldOfViewEditor;
    delete zNearEditor;
    delete zFarEditor;
}

void LensEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::Lens* lens = entity->GetComponent<Component::Lens>();
        SetVisible(lens != nullptr);
        
        if (IsVisible()) {
            fieldOfViewEditor->SetFloat(&lens->fieldOfView);
            zNearEditor->SetFloat(&lens->zNear);
            zFarEditor->SetFloat(&lens->zFar);
        }
    }
}
