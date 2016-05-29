#include "SelectHymnWindow.hpp"
#include <Engine/Resources.hpp>
#include <File.png.hpp>
#include <ABeeZee.ttf.hpp>

namespace GUI {
    SelectHymnWindow::SelectHymnWindow(Widget *parent) : Container(parent) {
        rectangle = Resources().CreateRectangle();
        
        hasClosedCallback = false;
        shouldClose = false;
        
        closeTexture = Resources().CreateTexture2D(FILE_PNG, FILE_PNG_LENGTH);
        closeButton = new ImageButton(this, closeTexture);
        closeButton->SetClickedCallback(std::bind(&Close, this));
        AddWidget(closeButton);
        
        font = Resources().CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
        
        nameTextField = new TextField(this, font);
        nameTextField->SetSize(glm::vec2(256.f, 32.f));
        AddWidget(nameTextField);
    }
    
    SelectHymnWindow::~SelectHymnWindow() {
        Resources().FreeRectangle();
        Resources().FreeFont(font);
        
        delete closeButton;
        Resources().FreeTexture2D(closeTexture);
        
        delete nameTextField;
    }
    
    void SelectHymnWindow::Update() {
        UpdateWidgets();
        
        if (shouldClose && hasClosedCallback)
            closedCallback();
            
    }
    
    void SelectHymnWindow::Render(const glm::vec2& screenSize) {
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        rectangle->Render(Position(), Size(), color, screenSize);
        
        RenderWidgets(screenSize);
    }
    
    glm::vec2 SelectHymnWindow::Size() const {
        return this->size;
    }
    
    void SelectHymnWindow::SetSize(const glm::vec2& size) {
        this->size = size;
        
        closeButton->SetPosition(Position() + glm::vec2(size.x - closeButton->Size().x, 0.f));
        nameTextField->SetPosition(Position() + glm::vec2(64.f, size.y - nameTextField->Size().y - 64.f));
    }
    
    void SelectHymnWindow::SetClosedCallback(std::function<void ()> callback) {
        closedCallback = callback;
        hasClosedCallback = true;
    }
    
    void SelectHymnWindow::Close() {
        shouldClose = true;
    }
}
