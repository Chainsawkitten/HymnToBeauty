#include "LensEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Lens.hpp>
#include "Label.hpp"
#include "FloatEditor.hpp"

using namespace GUI;

LensEditor::LensEditor(Widget* parent) : ComponentEditor(parent) {
    entity = nullptr;
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    lensLabel = new Label(this, font, "Lens");
    
    fieldOfViewLabel = new Label(this, font, "Field of view");
    fieldOfViewEditor = new FloatEditor(this, font);
    
    zNearLabel = new Label(this, font, "Z near");
    zNearEditor = new FloatEditor(this, font);
    
    zFarLabel = new Label(this, font, "Z far");
    zFarEditor = new FloatEditor(this, font);
    
    SetVisible(false);
}

LensEditor::~LensEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete lensLabel;
    
    delete fieldOfViewLabel;
    delete fieldOfViewEditor;
    
    delete zNearLabel;
    delete zNearEditor;
    
    delete zFarLabel;
    delete zFarEditor;
}

void LensEditor::Update() {
    if (IsVisible()) {
        fieldOfViewEditor->Update();
        zNearEditor->Update();
        zFarEditor->Update();
    }
}

void LensEditor::Render(const glm::vec2& screenSize) {
    if (IsVisible()) {
        lensLabel->Render(screenSize);
        
        fieldOfViewLabel->Render(screenSize);
        fieldOfViewEditor->Render(screenSize);
        
        zNearLabel->Render(screenSize);
        zNearEditor->Render(screenSize);
        
        zFarLabel->Render(screenSize);
        zFarEditor->Render(screenSize);
    }
}

void LensEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    glm::vec2 pos(position);
    
    lensLabel->SetPosition(pos);
    pos += glm::vec2(10.f, 20.f);
    
    fieldOfViewLabel->SetPosition(pos);
    pos.y += 20.f;
    fieldOfViewEditor->SetPosition(pos + glm::vec2(10.f, 0.f));
    pos.y += fieldOfViewEditor->GetSize().y + 5.f;
    
    zNearLabel->SetPosition(pos);
    pos.y += 20.f;
    zNearEditor->SetPosition(pos + glm::vec2(10.f, 0.f));
    pos.y += zNearEditor->GetSize().y + 5.f;
    
    zFarLabel->SetPosition(pos);
    pos.y += 20.f;
    zFarEditor->SetPosition(pos + glm::vec2(10.f, 0.f));
    pos.y += zFarEditor->GetSize().y + 5.f;
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
    this->entity = entity;
    
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
