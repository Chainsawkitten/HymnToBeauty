#include "EntityEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"
#include <Engine/Texture/Texture2D.hpp>
#include "Add.png.hpp"
#include "Subtract.png.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include "ComponentEditor/ComponentAdder.hpp"
#include "ComponentEditor/TransformEditor.hpp"
#include "ComponentEditor/LensEditor.hpp"
#include "ComponentEditor/MeshEditor.hpp"
#include "ComponentEditor/MaterialEditor.hpp"
#include "ComponentEditor/DirectionalLightEditor.hpp"
#include "ComponentEditor/PointLightEditor.hpp"
#include "ComponentEditor/SpotLightEditor.hpp"
#include "../Label.hpp"
#include "StringEditor.hpp"
#include <Engine/Entity/Entity.hpp>
#include "../ImageTextButton.hpp"

using namespace GUI;

EntityEditor::EntityEditor(Widget* parent, ModelSelector* modelSelector, TextureSelector* textureSelector, ComponentAdder* componentAdder) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    nameLabel = new Label(this, font, "Name");
    nameEditor = new StringEditor(this, font);
    
    addComponentTexture = Managers().resourceManager->CreateTexture2D(ADD_PNG, ADD_PNG_LENGTH);
    addComponentButton = new ImageTextButton(this, addComponentTexture, font, "Add component");
    addComponentButton->SetImageSize(glm::vec2(addComponentTexture->GetWidth(), addComponentTexture->GetHeight()));
    addComponentButton->SetClickedCallback(std::bind(&AddComponentPressed, this));
    
    removeEntityTexture = Managers().resourceManager->CreateTexture2D(SUBTRACT_PNG, SUBTRACT_PNG_LENGTH);
    removeEntityButton = new ImageTextButton(this, removeEntityTexture, font, "Delete entity");
    removeEntityButton->SetImageSize(glm::vec2(removeEntityTexture->GetWidth(), removeEntityTexture->GetHeight()));
    removeEntityButton->SetClickedCallback(std::bind(&RemoveEntityPressed, this));
    
    editors.push_back(new TransformEditor(this));
    editors.push_back(new LensEditor(this));
    editors.push_back(new MeshEditor(this, modelSelector));
    editors.push_back(new MaterialEditor(this, textureSelector));
    editors.push_back(new DirectionalLightEditor(this));
    editors.push_back(new PointLightEditor(this));
    editors.push_back(new SpotLightEditor(this));
    
    this->componentAdder = componentAdder;
    
    SetVisible(false);
}

EntityEditor::~EntityEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete nameLabel;
    delete nameEditor;
    
    Managers().resourceManager->FreeTexture2D(addComponentTexture);
    delete addComponentButton;
    
    Managers().resourceManager->FreeTexture2D(removeEntityTexture);
    delete removeEntityButton;
    
    for (ComponentEditor* editor : editors)
        delete editor;
}

void EntityEditor::Update() {
    nameEditor->Update();
    addComponentButton->Update();
    removeEntityButton->Update();
    
    for (ComponentEditor* editor : editors)
        editor->Update();
}

void EntityEditor::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    nameLabel->Render(screenSize);
    nameEditor->Render(screenSize);
    addComponentButton->Render(screenSize);
    removeEntityButton->Render(screenSize);
    
    for (ComponentEditor* editor : editors)
        editor->Render(screenSize);
}

void EntityEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    glm::vec2 pos(position);
    
    nameLabel->SetPosition(pos);
    nameEditor->SetPosition(pos + glm::vec2(10.f, 20.f));
    pos.y += 50.f;
    removeEntityButton->SetPosition(pos);
    pos.y += 20.f;
    addComponentButton->SetPosition(pos);
    pos.y += 30.f;
    
    for (ComponentEditor* editor : editors) {
        editor->SetPosition(pos);
        
        if (editor->IsVisible())
            pos.y += editor->GetSize().y + 10.f;
    }
}

glm::vec2 EntityEditor::GetSize() const {
    return size;
}

void EntityEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    nameEditor->SetSize(glm::vec2(size.x - 10.f, 20.f));
    addComponentButton->SetSize(glm::vec2(size.x, 20.f));
    removeEntityButton->SetSize(glm::vec2(size.x, 20.f));
    
    for (ComponentEditor* editor : editors)
        editor->SetSize(size);
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    nameEditor->SetString(&entity->name);
    
    for (ComponentEditor* editor : editors)
        editor->SetEntity(entity);
    
    // Update editor positions.
    SetPosition(GetPosition());
}

void EntityEditor::AddComponentPressed() {
    componentAdder->SetEntity(entity);
    componentAdder->SetComponentAddedCallback(std::bind(&SetEntity, this, entity));
    componentAdder->SetVisible(true);
}

void EntityEditor::RemoveEntityPressed() {
    entity->Kill();
    SetVisible(false);
}
