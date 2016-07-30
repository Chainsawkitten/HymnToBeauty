#include "TransformEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>
#include "FloatEditor.hpp"
#include "Label.hpp"

using namespace GUI;

TransformEditor::TransformEditor(Widget* parent) : Widget(parent) {
    entity = nullptr;
    rectangle = Managers().resourceManager->CreateRectangle();
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    // Position
    xLabel = new Label(this, font, "X:");
    xLabel->SetPosition(GetPosition());
    
    xEditor = new FloatEditor(this, font);
    xEditor->SetPosition(GetPosition());
    
    yLabel = new Label(this, font, "Y:");
    yLabel->SetPosition(GetPosition());
    
    yEditor = new FloatEditor(this, font);
    yEditor->SetPosition(GetPosition());
    
    zLabel = new Label(this, font, "Z:");
    zLabel->SetPosition(GetPosition());
    
    zEditor = new FloatEditor(this, font);
    zEditor->SetPosition(GetPosition());
}

TransformEditor::~TransformEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete xLabel;
    delete xEditor;
    delete yLabel;
    delete yEditor;
    delete zLabel;
    delete zEditor;
}

void TransformEditor::Update() {
    xEditor->Update();
    yEditor->Update();
    zEditor->Update();
}

void TransformEditor::Render(const glm::vec2& screenSize) {
    if (entity != nullptr) {
        xLabel->Render(screenSize);
        xEditor->Render(screenSize);
        yLabel->Render(screenSize);
        yEditor->Render(screenSize);
        zLabel->Render(screenSize);
        zEditor->Render(screenSize);
    }
}

void TransformEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    xLabel->SetPosition(position);
    xEditor->SetPosition(position + glm::vec2(xLabel->GetSize().x, 0.f));
    
    yLabel->SetPosition(position + glm::vec2(0.f, 20.f));
    yEditor->SetPosition(position + glm::vec2(yLabel->GetSize().x, 20.f));
    
    zLabel->SetPosition(position+ glm::vec2(0.f, 40.f));
    zEditor->SetPosition(position + glm::vec2(zLabel->GetSize().x, 40.f));
}

glm::vec2 TransformEditor::GetSize() const {
    return size;
}

void TransformEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    xEditor->SetSize(glm::vec2(size.x - xLabel->GetSize().x, 20.f));
    yEditor->SetSize(glm::vec2(size.x - yLabel->GetSize().x, 20.f));
    zEditor->SetSize(glm::vec2(size.x - zLabel->GetSize().x, 20.f));
}

void TransformEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    Component::Transform* transform = entity->GetComponent<Component::Transform>();
    xEditor->SetFloat(&transform->position.x);
    yEditor->SetFloat(&transform->position.y);
    zEditor->SetFloat(&transform->position.z);
}
