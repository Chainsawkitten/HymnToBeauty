#include "MaterialEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "../../TextButton.hpp"
#include "../../TextureSelector.hpp"

using namespace GUI;

MaterialEditor::MaterialEditor(Widget* parent, TextureSelector* textureSelector) : ComponentEditor(parent, "Material") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::Material>();
    
    selectDiffuseButton = new TextButton(this, font, "Select diffuse texture");
    selectDiffuseButton->SetClickedCallback(std::bind(&SelectDiffusePressed, this));
    AddEditor("Diffuse", selectDiffuseButton);
    
    selectNormalButton = new TextButton(this, font, "Select normal texture");
    selectNormalButton->SetClickedCallback(std::bind(&SelectNormalPressed, this));
    AddEditor("Normal", selectNormalButton);
    
    selectSpecularButton = new TextButton(this, font, "Select specular texture");
    selectSpecularButton->SetClickedCallback(std::bind(&SelectSpecularPressed, this));
    AddEditor("Specular", selectSpecularButton);
    
    selectGlowButton = new TextButton(this, font, "Select glow texture");
    selectGlowButton->SetClickedCallback(std::bind(&SelectGlowPressed, this));
    AddEditor("Glow", selectGlowButton);
    
    this->textureSelector = textureSelector;
    
    SetVisible(false);
}

MaterialEditor::~MaterialEditor() {
    Managers().resourceManager->FreeFont(font);
}

void MaterialEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        material = entity->GetComponent<Component::Material>();
        SetVisible(material != nullptr);
    }
}

void MaterialEditor::SelectDiffusePressed() {
    textureSelector->SetVisible(true);
    textureSelector->UpdateTextures();
    textureSelector->SetTextureSelectedCallback(std::bind(&DiffuseSelected, this, std::placeholders::_1));
}

void MaterialEditor::DiffuseSelected(Texture2D* texture) {
    material->diffuse = texture;
}

void MaterialEditor::SelectNormalPressed() {
    textureSelector->SetVisible(true);
    textureSelector->UpdateTextures();
    textureSelector->SetTextureSelectedCallback(std::bind(&NormalSelected, this, std::placeholders::_1));
}

void MaterialEditor::NormalSelected(Texture2D* texture) {
    material->normal = texture;
}

void MaterialEditor::SelectSpecularPressed() {
    textureSelector->SetVisible(true);
    textureSelector->UpdateTextures();
    textureSelector->SetTextureSelectedCallback(std::bind(&SpecularSelected, this, std::placeholders::_1));
}

void MaterialEditor::SpecularSelected(Texture2D* texture) {
    material->specular = texture;
}

void MaterialEditor::SelectGlowPressed() {
    textureSelector->SetVisible(true);
    textureSelector->UpdateTextures();
    textureSelector->SetTextureSelectedCallback(std::bind(&GlowSelected, this, std::placeholders::_1));
}

void MaterialEditor::GlowSelected(Texture2D* texture) {
    material->glow = texture;
}
