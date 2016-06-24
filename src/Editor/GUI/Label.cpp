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

void Label::Render(const glm::vec2& screenSize) {
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText(text.c_str(), GetPosition(), std::numeric_limits<float>::max(), screenSize);
}

glm::vec2 Label::GetSize() const {
    /// @todo Get text size.
    return glm::vec2(0.f, 0.f);
}

void Label::SetSize(const glm::vec2& size) {
    // Do nothing.
}

void Label::SetText(const std::string& text) {
    this->text = text;
}
