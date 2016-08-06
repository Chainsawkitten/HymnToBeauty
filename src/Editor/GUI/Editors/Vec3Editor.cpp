#include "Vec3Editor.hpp"

#include "../Label.hpp"
#include "FloatEditor.hpp"

using namespace GUI;

Vec3Editor::Vec3Editor(Widget* parent, Font* font) : Widget(parent) {
    this->font = font;
    
    xLabel = new Label(this, font, "X");
    xLabel->SetPosition(GetPosition());
    
    xEditor = new FloatEditor(this, font);
    xEditor->SetPosition(GetPosition());
    
    yLabel = new Label(this, font, "Y");
    yLabel->SetPosition(GetPosition());
    
    yEditor = new FloatEditor(this, font);
    yEditor->SetPosition(GetPosition());
    
    zLabel = new Label(this, font, "Z");
    zLabel->SetPosition(GetPosition());
    
    zEditor = new FloatEditor(this, font);
    zEditor->SetPosition(GetPosition());
}

Vec3Editor::~Vec3Editor() {
    delete xLabel;
    delete xEditor;
    delete yLabel;
    delete yEditor;
    delete zLabel;
    delete zEditor;
}

void Vec3Editor::Update() {
    xEditor->Update();
    yEditor->Update();
    zEditor->Update();
}

void Vec3Editor::Render(const glm::vec2& screenSize) {
    if (variable != nullptr) {
        xLabel->Render(screenSize);
        xEditor->Render(screenSize);
        yLabel->Render(screenSize);
        yEditor->Render(screenSize);
        zLabel->Render(screenSize);
        zEditor->Render(screenSize);
    }
}

void Vec3Editor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    xLabel->SetPosition(position);
    xEditor->SetPosition(position + glm::vec2(20.f, 0.f));
    
    yLabel->SetPosition(position + glm::vec2(0.f, 20.f));
    yEditor->SetPosition(position + glm::vec2(20.f, 20.f));
    
    zLabel->SetPosition(position+ glm::vec2(0.f, 40.f));
    zEditor->SetPosition(position + glm::vec2(20.f, 40.f));
}

glm::vec2 Vec3Editor::GetSize() const {
    return size;
}

void Vec3Editor::SetSize(const glm::vec2& size) {
    this->size = size;
    this->size.y = 60.f;
    
    xEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
    yEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
    zEditor->SetSize(glm::vec2(size.x - 20.f, 20.f));
}

void Vec3Editor::SetVec3(glm::vec3* variable) {
    this->variable = variable;
    
    xEditor->SetFloat(&variable->x);
    yEditor->SetFloat(&variable->y);
    zEditor->SetFloat(&variable->z);
}
