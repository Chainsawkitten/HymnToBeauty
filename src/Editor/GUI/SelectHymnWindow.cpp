#include "SelectHymnWindow.hpp"
#include <Engine/Resources.hpp>
#include <Close.png.hpp>
#include <ABeeZee.ttf.hpp>

using namespace GUI;
using namespace std;

SelectHymnWindow::SelectHymnWindow(Widget *parent) : Container(parent) {
    rectangle = Resources().CreateRectangle();
    
    hasClosedCallback = false;
    shouldClose = false;
    
    closeTexture = Resources().CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&Close, this));
    AddWidget(closeButton);
    
    font = Resources().CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    selectButton = new TextButton(this, font, "Select");
    selectButton->SetClickedCallback(std::bind(&Select, this));
    selectButton->SetSize(glm::vec2(64.f, 32.f));
    AddWidget(selectButton);
    
    nameTextField = new TextField(this, font);
    nameTextField->SetSize(glm::vec2(256.f, 32.f));
    AddWidget(nameTextField);
    
    hymnList = new VerticalLayout(this);
    AddWidget(hymnList);
}

SelectHymnWindow::~SelectHymnWindow() {
    Resources().FreeRectangle();
    Resources().FreeFont(font);
    
    delete closeButton;
    Resources().FreeTexture2D(closeTexture);
    delete selectButton;
    
    delete nameTextField;
    delete hymnList;
}

void SelectHymnWindow::Update() {
    UpdateWidgets();
    
    if (shouldClose && hasClosedCallback)
        closedCallback(nameTextField->Text());
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
    nameTextField->SetPosition(Position() + glm::vec2(32.f, size.y - nameTextField->Size().y - 32.f));
    nameTextField->SetSize(glm::vec2(size.x - 96.f - selectButton->Size().x, 32.f));
    selectButton->SetPosition(Position() + glm::vec2(size.x - selectButton->Size().x - 32.f, size.y - selectButton->Size().y - 32.f));
    hymnList->SetPosition(Position() + glm::vec2(0.f, closeButton->Size().y + 32.f));
    hymnList->SetSize(glm::vec2(size.x, nameTextField->Position().y - closeButton->Size().y - 64.f));
}

void SelectHymnWindow::SetClosedCallback(std::function<void(const std::string&)> callback) {
    closedCallback = callback;
    hasClosedCallback = true;
}

void SelectHymnWindow::Close() {
    shouldClose = true;
    nameTextField->SetText("");
}

void SelectHymnWindow::Select() {
    shouldClose = true;
}
