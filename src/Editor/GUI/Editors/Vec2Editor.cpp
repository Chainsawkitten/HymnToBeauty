#include "Vec2Editor.hpp"

#include "../Label.hpp"
#include "FloatEditor.hpp"

using namespace GUI;

Vec2Editor::Vec2Editor(Widget* parent, Font* font) : Widget(parent) {
    this->font = font;
    
    xLabel = new Label(this, font, "X");
    xLabel->SetPosition(GetPosition());
    
    xEditor = new FloatEditor(this, font);
    xEditor->SetPosition(GetPosition());
    
    yLabel = new Label(this, font, "Y");
    yLabel->SetPosition(GetPosition());
    
    yEditor = new FloatEditor(this, font);
    yEditor->SetPosition(GetPosition());
}

Vec2Editor::~Vec2Editor() {
    delete xLabel;
    delete xEditor;
    delete yLabel;
    delete yEditor;
}

void Vec2Editor::Update() {
    xEditor->Update();
    yEditor->Update();
}

void Vec2Editor::Render() {
    if (variable != nullptr) {
        xLabel->Render();
        xEditor->Render();
        yLabel->Render();
        yEditor->Render();
    }
}

void Vec2Editor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    xLabel->SetPosition(position);
    xEditor->SetPosition(position + glm::vec2(20.f, 0.f));
    
    yLabel->SetPosition(position + glm::vec2(0.f, 20.f));
    yEditor->SetPosition(position + glm::vec2(20.f, 20.f));
}

glm::vec2 Vec2Editor::GetSize() const {
    return size;
}

void Vec2Editor::SetSize(const glm::vec2& size) {
    this->size = size;
    this->size.y = 40.f;
    
    xEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
    yEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
}

void Vec2Editor::SetVec2(glm::vec2* variable) {
    this->variable = variable;
    
    xEditor->SetFloat(&variable->x);
    yEditor->SetFloat(&variable->y);
}
