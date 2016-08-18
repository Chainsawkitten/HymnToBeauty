#include "ImageTextButton.hpp"

#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Font/Font.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/Input.hpp>

using namespace GUI;

ImageTextButton::ImageTextButton(Widget* parent, Texture2D* texture, Font* font, const std::string& text) : Button(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    this->texture = texture;
    this->font = font;
    this->text = text;
    
    imageSize = glm::vec2(64.f, 64.f);
}

ImageTextButton::~ImageTextButton() {
    Managers().resourceManager->FreeRectangle();
}

void ImageTextButton::Render(const glm::vec2& screenSize) {
    // Draw background.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    if (GetMouseHover())
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
    
    rectangle->Render(GetPosition(), GetSize(), color);
    
    // Draw texture
    texture->Render(GetPosition() + glm::vec2(0.f, (GetSize().y - imageSize.y) * 0.5f), imageSize, screenSize);
    
    // Draw text
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText(text.c_str(), GetPosition() + glm::vec2(imageSize.x, 0.5f * (GetSize().y - font->GetHeight())), GetSize().x - imageSize.x, screenSize);
}

glm::vec2 ImageTextButton::GetImageSize() const {
    return imageSize;
}

void ImageTextButton::SetImageSize(const glm::vec2 &size) {
    imageSize = size;
}
