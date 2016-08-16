#include "DirectionalLightEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include "../Vec3Editor.hpp"
#include "../FloatEditor.hpp"

using namespace GUI;

DirectionalLightEditor::DirectionalLightEditor(Widget* parent) : ComponentEditor(parent, "Directional light") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::DirectionalLight>();
    
    colorEditor = new Vec3Editor(this, font);
    AddEditor("Color", colorEditor);
    
    ambientCoefficientEditor = new FloatEditor(this, font);
    AddEditor("Ambient coefficient", ambientCoefficientEditor);
    
    SetVisible(false);
}

DirectionalLightEditor::~DirectionalLightEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete colorEditor;
    delete ambientCoefficientEditor;
}

void DirectionalLightEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::DirectionalLight* directionalLight = entity->GetComponent<Component::DirectionalLight>();
        SetVisible(directionalLight != nullptr);
        
        if (IsVisible()) {
            colorEditor->SetVec3(&directionalLight->color);
            ambientCoefficientEditor->SetFloat(&directionalLight->ambientCoefficient);
        }
    }
}
