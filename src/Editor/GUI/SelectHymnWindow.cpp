#include "SelectHymnWindow.hpp"

#include "ImageButton.hpp"
#include "TextButton.hpp"
#include "TextField.hpp"
#include "VerticalLayout.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <OpenHymn.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Util/FileSystem.hpp>
#include "ImageTextButton.hpp"

using namespace GUI;
using namespace std;

SelectHymnWindow::SelectHymnWindow(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    selectButton = new TextButton(this, font, "Select");
    selectButton->SetClickedCallback(std::bind(&Select, this));
    selectButton->SetSize(glm::vec2(64.f, 32.f));
    AddWidget(selectButton);
    
    nameTextField = new TextField(this, font);
    nameTextField->SetSize(glm::vec2(256.f, 32.f));
    AddWidget(nameTextField);
    
    hymnList = new VerticalLayout(this);
    AddWidget(hymnList);
    
    hymnTexture = Managers().resourceManager->CreateTexture2D(OPENHYMN_PNG, OPENHYMN_PNG_LENGTH);
    vector<string> files = FileSystem::DirectoryContents(FileSystem::DataPath("Hymn to Beauty"), FileSystem::DIRECTORY);
    for (string file : files) {
        ImageTextButton* hymn = new ImageTextButton(this, hymnTexture, font, file);
        hymn->SetClickedCallback(std::bind(&SetHymn, this, file));
        hymnList->AddWidget(hymn);
    }
}

SelectHymnWindow::~SelectHymnWindow() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    delete selectButton;
    
    delete nameTextField;
    
    for (Widget* hymn : hymnList->GetWidgets())
        delete hymn;
    
    delete hymnList;
    
    Managers().resourceManager->FreeTexture2D(hymnTexture);
}

void SelectHymnWindow::Update() {
    UpdateWidgets();
    
    if (shouldClose && hasClosedCallback)
        closedCallback(nameTextField->GetText());
}

void SelectHymnWindow::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), GetSize(), color);
    
    RenderWidgets();
}

glm::vec2 SelectHymnWindow::GetSize() const {
    return size;
}

void SelectHymnWindow::SetSize(const glm::vec2& size) {
    this->size = size;
    
    closeButton->SetPosition(GetPosition() + glm::vec2(size.x - closeButton->GetSize().x, 0.f));
    nameTextField->SetPosition(GetPosition() + glm::vec2(32.f, size.y - nameTextField->GetSize().y - 32.f));
    nameTextField->SetSize(glm::vec2(size.x - 96.f - selectButton->GetSize().x, 32.f));
    selectButton->SetPosition(GetPosition() + glm::vec2(size.x - selectButton->GetSize().x - 32.f, size.y - selectButton->GetSize().y - 32.f));
    
    for (Widget* hymn : hymnList->GetWidgets())
        hymn->SetSize(glm::vec2(size.x, 64.f));
    
    hymnList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    hymnList->SetSize(glm::vec2(size.x, nameTextField->GetPosition().y - closeButton->GetSize().y - 64.f));
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

void SelectHymnWindow::SetHymn(const string& name) {
    nameTextField->SetText(name);
}
