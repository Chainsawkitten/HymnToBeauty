#include "PointLightEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/PointLight.hpp>
#include "../Vec3Editor.hpp"
#include "../FloatEditor.hpp"

using namespace GUI;

PointLightEditor::PointLightEditor(Widget* parent) : ComponentEditor(parent, "Point light") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::PointLight>();
    
    colorEditor = new Vec3Editor(this, font);
    AddEditor("Color", colorEditor);
    
    ambientCoefficientEditor = new FloatEditor(this, font);
    AddEditor("Ambient coefficient", ambientCoefficientEditor);
    
    attenuationEditor = new FloatEditor(this, font);
    AddEditor("Attenuation", attenuationEditor);
    
    intensityEditor = new FloatEditor(this, font);
    AddEditor("Intensity", intensityEditor);
    
    SetVisible(false);
}

PointLightEditor::~PointLightEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete colorEditor;
    delete ambientCoefficientEditor;
    delete attenuationEditor;
    delete intensityEditor;
}

void PointLightEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::PointLight* pointLight = entity->GetComponent<Component::PointLight>();
        SetVisible(pointLight != nullptr);
        
        if (IsVisible()) {
            colorEditor->SetVec3(&pointLight->color);
            ambientCoefficientEditor->SetFloat(&pointLight->ambientCoefficient);
            attenuationEditor->SetFloat(&pointLight->attenuation);
            intensityEditor->SetFloat(&pointLight->intensity);
        }
    }
}
