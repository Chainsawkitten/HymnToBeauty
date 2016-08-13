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
        markerPosition = text.length();
    }
    
    if (focus) {
        bool textUpdated = !Input()->Text().empty();
        
        text = text.insert(markerPosition, Input()->Text());
        markerPosition += Input()->Text().length();
        
        // Erase previous character when BACK pressed.
        if (Input()->Triggered(InputHandler::BACK) && markerPosition > 0U) {
            text = text.erase(markerPosition-1, 1);
            --markerPosition;
            textUpdated = true;
        }
        
        // Erase next character when ERASE pressed.
        if (Input()->Triggered(InputHandler::ERASE) && markerPosition < text.length()) {
            text = text.erase(markerPosition, 1);
            textUpdated = true;
        }
        
        // Move marker.
        if (Input()->Triggered(InputHandler::LEFT) && markerPosition > 0U)
            --markerPosition;
        
        if (Input()->Triggered(InputHandler::RIGHT) && markerPosition < text.length())
            ++markerPosition;
        
        if (textUpdated)
            TextUpdated();
    }
}

void TextField::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.16078431372f, 0.15686274509f, 0.17647058823f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText(text.c_str(), GetPosition(), size.x, screenSize);
    
    if (focus)
        rectangle->Render(GetPosition() + glm::vec2(font->GetWidth(text.substr(0, markerPosition).c_str()), 0.f), glm::vec2(1, size.y), glm::vec3(1.f, 1.f, 1.f), screenSize);
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
