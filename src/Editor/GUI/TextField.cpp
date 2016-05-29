#include "TextField.hpp"
#include <Engine/Resources.hpp>
#include <Engine/Util/Input.hpp>

namespace GUI {
    TextField::TextField(Widget *parent, Font* font) : Widget(parent) {
        rectangle = Resources().CreateRectangle();
        this->font = font;
        text = "";
    }
    
    TextField::~TextField() {
        Resources().FreeRectangle();
    }
    
    void TextField::Update() {
        text += Input()->Text();
    }
    
    void TextField::Render(const glm::vec2& screenSize) {
        glm::vec3 color(0.16078431372f, 0.15686274509f, 0.17647058823f);
        rectangle->Render(Position(), size, color, screenSize);
        
        font->SetColor(glm::vec3(1.f, 1.f, 1.f));
        font->RenderText(text.c_str(), Position(), size.x, screenSize);
    }
    
    glm::vec2 TextField::Size() const {
        return size;
    }
    
    void TextField::SetSize(const glm::vec2 &size) {
        this->size = size;
    }
    
    std::string TextField::Text() const {
        return text;
    }
    
    void TextField::SetText(const std::string& text) {
        this->text = text;
    }
}
