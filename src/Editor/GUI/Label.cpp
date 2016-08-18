#include "Label.hpp"

#include <Engine/Font/Font.hpp>
#include <limits>

using namespace GUI;

Label::Label(Widget* parent, Font* font, const std::string& text) : Widget(parent) {
    this->font = font;
    this->text = text;
}

void Label::Update() {
    
}

void Label::Render() {
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText(text.c_str(), GetPosition(), std::numeric_limits<float>::max());
}

glm::vec2 Label::GetSize() const {
    return glm::vec2(font->GetWidth(text.c_str()), font->GetHeight());
}

void Label::SetSize(const glm::vec2& size) {
    // Do nothing.
}

void Label::SetText(const std::string& text) {
    this->text = text;
}
