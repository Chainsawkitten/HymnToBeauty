#include "ImageButton.hpp"

#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Resources.hpp>
#include <Engine/Util/Input.hpp>

using namespace GUI;

ImageButton::ImageButton(Widget* parent, Texture2D* texture) : Button(parent) {
    rectangle = Resources().CreateRectangle();
    this->texture = texture;
}

ImageButton::~ImageButton() {
    Resources().FreeRectangle();
}

void ImageButton::Render(const glm::vec2& screenSize) {
    // Draw background.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    if (GetMouseHover())
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
    
    rectangle->Render(GetPosition(), GetSize(), color, screenSize);
    
    // Draw image.
    texture->Render(GetPosition(), GetSize(), screenSize);
}
