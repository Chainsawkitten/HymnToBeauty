#include "Label.hpp"

#include <Engine/Resources.hpp>
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
    font->RenderText(text.c_str(), Position(), std::numeric_limits<float>::max(), screenSize);
}

glm::vec2 Label::Size() const {
    return glm::vec2(0.f, 0.f);
}

void Label::SetText(const std::string& text) {
    this->text = text;
}
