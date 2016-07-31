#include "LensEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Lens.hpp>
#include "FloatEditor.hpp"

using namespace GUI;

LensEditor::LensEditor(Widget* parent) : ComponentEditor(parent, "Lens") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
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

glm::vec2 LensEditor::GetSize() const {
    return size;
}

void LensEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    fieldOfViewEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
    zNearEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
    zFarEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
    
    this->size.y = 130.f;
}

void LensEditor::SetEntity(Entity* entity) {
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
