#include "ImageButton.hpp"

#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/Input.hpp>

using namespace GUI;

ImageButton::ImageButton(Widget* parent, Texture2D* texture) : Button(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    this->texture = texture;
}

ImageButton::~ImageButton() {
    Managers().resourceManager->FreeRectangle();
}

void ImageButton::Render() {
    // Draw background.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    if (GetMouseHover())
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
    
    rectangle->Render(GetPosition(), GetSize(), color);
    
    // Draw image.
    texture->Render(GetPosition(), GetSize());
}
