#include "TextButton.hpp"
#include <Engine/Resources.hpp>
#include <Engine/Util/Input.hpp>

using namespace GUI;

TextButton::TextButton(Widget* parent, Font* font, const std::string& text) : Button(parent) {
    rectangle = Resources().CreateRectangle();
    
    this->font = font;
    this->text = text;
}

TextButton::~TextButton() {
    Resources().FreeRectangle();
}

void TextButton::Render(const glm::vec2& screenSize) {
    // Draw background.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    if (MouseHover())
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
    
    rectangle->Render(Position(), Size(), color, screenSize);
    
    // Draw text
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText(text.c_str(), Position() + glm::vec2(0.f, 0.5f * (Size().y - font->Height())), Size().x, screenSize);
}
