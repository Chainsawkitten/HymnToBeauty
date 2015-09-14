#include "ImageTextButton.hpp"
#include <Core/Resources.hpp>
#include <Core/Util/Input.hpp>

namespace GUI {
    ImageTextButton::ImageTextButton(Widget* parent, Texture2D* texture, Font* font, const std::string& text) : Button(parent) {
        rectangle = Resources().CreateRectangle();
        
        this->texture = texture;
        this->font = font;
        this->text = text;
        
        imageSize = glm::vec2(64.f, 64.f);
    }
    
    ImageTextButton::~ImageTextButton() {
        Resources().FreeRectangle();
    }
    
    void ImageTextButton::Render(int screenWidth, int screenHeight) {
        // Draw background.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        if (MouseHover())
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        
        rectangle->Render(Position(), Size(), color, screenWidth, screenHeight);
        
        // Draw texture
        texture->Render(Position(), imageSize, screenWidth, screenHeight);
        
        // Draw text
        font->SetColor(glm::vec3(1.f, 1.f, 1.f));
        font->RenderText(text.c_str(), Position() + glm::vec2(imageSize.x, 0.5f * (Size().y - font->Height())), Size().x - imageSize.x, screenWidth, screenHeight);
    }
    
    glm::vec2 ImageTextButton::ImageSize() const {
        return imageSize;
    }
    
    void ImageTextButton::SetImageSize(const glm::vec2 &size) {
        imageSize = size;
    }
}
