#include "TextField.hpp"

#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Font/Font.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Physics/Rectangle.hpp>

using namespace GUI;

TextField::TextField(Widget *parent, Font* font) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    this->font = font;
}

TextField::~TextField() {
    Managers().resourceManager->FreeRectangle();
}

void TextField::Update() {
    if (Input()->Triggered(InputHandler::CLICK)) {
        glm::vec2 mousePosition(Input()->CursorX(), Input()->CursorY());
        Physics::Rectangle rect(GetPosition(), size);
        focus = rect.Collide(mousePosition);
    }
    
    if (focus) {
        bool textUpdated = !Input()->Text().empty();
        
        text += Input()->Text();
        
        if (Input()->Triggered(InputHandler::BACK) && text.length() > 0) {
            text = text.erase(text.length()-1, 1);
            textUpdated = true;
        }
        
        if (textUpdated)
            TextUpdated();
    }
}

void TextField::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.16078431372f, 0.15686274509f, 0.17647058823f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText(text.c_str(), GetPosition(), size.x, screenSize);
}

glm::vec2 TextField::GetSize() const {
    return size;
}

void TextField::SetSize(const glm::vec2& size) {
    this->size = size;
}

std::string TextField::GetText() const {
    return text;
}

void TextField::SetText(const std::string& text) {
    this->text = text;
}

void TextField::TextUpdated() {
    
}
