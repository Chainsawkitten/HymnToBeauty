#include "EntityEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include "ComponentEditor/TransformEditor.hpp"
#include "ComponentEditor/LensEditor.hpp"

using namespace GUI;

EntityEditor::EntityEditor(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    entity = nullptr;
    editors.push_back(new TransformEditor(this));
    editors.push_back(new LensEditor(this));
}

EntityEditor::~EntityEditor() {
    Managers().resourceManager->FreeRectangle();
    
    for (ComponentEditor* editor : editors)
        delete editor;
}

void EntityEditor::Update() {
    for (ComponentEditor* editor : editors)
        editor->Update();
}

void EntityEditor::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    for (ComponentEditor* editor : editors)
        editor->Render(screenSize);
}

void EntityEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    glm::vec2 pos(position);
    
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
    
    for (ComponentEditor* editor : editors)
        editor->SetSize(size);
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    for (ComponentEditor* editor : editors)
        editor->SetEntity(entity);
    
    // Update editor positions.
    SetPosition(GetPosition());
}
