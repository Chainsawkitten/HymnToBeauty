#include "SelectHymnWindow.hpp"
#include <Core/Resources.hpp>
#include <File.png.hpp>

namespace GUI {
    SelectHymnWindow::SelectHymnWindow(Widget *parent) : Container(parent) {
        rectangle = Resources().CreateRectangle();
        
        hasClosedCallback = false;
        
        closeTexture = Resources().CreateTexture2D(FILE_PNG, FILE_PNG_LENGTH);
        closeButton = new ImageButton(this, closeTexture);
        closeButton->SetClickedCallback(std::bind(&Close, this));
        AddWidget(closeButton);
    }
    
    SelectHymnWindow::~SelectHymnWindow() {
        Resources().FreeRectangle();
        
        delete closeButton;
        Resources().FreeTexture2D(closeTexture);
    }
    
    void SelectHymnWindow::Render(const glm::vec2 &screenSize) {
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        rectangle->Render(Position(), Size(), color, screenSize);
        
        RenderWidgets(screenSize);
    }
    
    glm::vec2 SelectHymnWindow::Size() const {
        return this->size;
    }
    
    void SelectHymnWindow::SetSize(const glm::vec2& size) {
        this->size = size;
        
        closeButton->SetPosition(glm::vec2(size.x - closeButton->Size().x, 0.f));
    }
    
    void SelectHymnWindow::SetClosedCallback(std::function<void ()> callback) {
        closedCallback = callback;
        hasClosedCallback = true;
    }
    
    void SelectHymnWindow::Close() {
        if (hasClosedCallback)
            closedCallback();
    }
}
