#include "SpotLightEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/SpotLight.hpp>
#include "../Vec3Editor.hpp"
#include "../FloatEditor.hpp"

using namespace GUI;

SpotLightEditor::SpotLightEditor(Widget* parent) : ComponentEditor(parent, "Point light") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    colorEditor = new Vec3Editor(this, font);
    AddEditor("Color", colorEditor);
    
    ambientCoefficientEditor = new FloatEditor(this, font);
    AddEditor("Ambient coefficient", ambientCoefficientEditor);
    
    attenuationEditor = new FloatEditor(this, font);
    AddEditor("Attenuation", attenuationEditor);
    
    intensityEditor = new FloatEditor(this, font);
    AddEditor("Intensity", intensityEditor);
    
    coneAngleEditor = new FloatEditor(this, font);
    AddEditor("Cone angle", coneAngleEditor);
    
    SetVisible(false);
}

SpotLightEditor::~SpotLightEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete colorEditor;
    delete ambientCoefficientEditor;
    delete attenuationEditor;
    delete intensityEditor;
    delete coneAngleEditor;
}

void SpotLightEditor::SetEntity(Entity* entity) {
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::SpotLight* spotLight = entity->GetComponent<Component::SpotLight>();
        SetVisible(spotLight != nullptr);
        
        if (IsVisible()) {
            colorEditor->SetVec3(&spotLight->color);
            ambientCoefficientEditor->SetFloat(&spotLight->ambientCoefficient);
            attenuationEditor->SetFloat(&spotLight->attenuation);
            intensityEditor->SetFloat(&spotLight->intensity);
            coneAngleEditor->SetFloat(&spotLight->coneAngle);
        }
    }
}
