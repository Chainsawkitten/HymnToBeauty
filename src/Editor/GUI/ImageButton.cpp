#include "ImageButton.hpp"
#include <Core/Resources.hpp>
#include <Core/Util/Input.hpp>

namespace GUI {
    ImageButton::ImageButton(Widget* parent, Texture2D* texture) : Button(parent) {
        rectangle = Resources().CreateRectangle();
        this->texture = texture;
    }
    
    ImageButton::~ImageButton() {
        Resources().FreeRectangle();
    }
    
    void ImageButton::Render(int screenWidth, int screenHeight) {
        // Draw background.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        if (MouseHover())
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        
        rectangle->Render(Position(), Size(), color, screenWidth, screenHeight);
        
        // Draw image.
        texture->Render(Position(), Size(), screenWidth, screenHeight);
    }
}
