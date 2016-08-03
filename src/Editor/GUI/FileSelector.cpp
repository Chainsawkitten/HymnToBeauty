#include "FileSelector.hpp"

#include "ImageButton.hpp"
#include "TextButton.hpp"
#include "VerticalScrollLayout.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <OpenHymn.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Util/FileSystem.hpp>

using namespace GUI;
using namespace std;

FileSelector::FileSelector(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    hasClosedCallback = false;
    shouldClose = false;
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    selectButton = new TextButton(this, font, "Select");
    selectButton->SetClickedCallback(std::bind(&Select, this));
    selectButton->SetSize(glm::vec2(64.f, 32.f));
    AddWidget(selectButton);
    
    fileList = new VerticalScrollLayout(this);
    AddWidget(fileList);
    
    path = FileSystem::DataPath("Hymn to Beauty");
    pathChanged = false;
    ScanDirectory();
}

FileSelector::~FileSelector() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    delete selectButton;
    
    fileList->ClearWidgets();
    delete fileList;
}

void FileSelector::Update() {
    if (pathChanged) {
        ScanDirectory();
        pathChanged = false;
    }
    
    UpdateWidgets();
    
    if (shouldClose && hasClosedCallback)
        closedCallback("");
}

void FileSelector::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), GetSize(), color, screenSize);
    
    RenderWidgets(screenSize);
}

glm::vec2 FileSelector::GetSize() const {
    return size;
}

void FileSelector::SetSize(const glm::vec2& size) {
    this->size = size;
    
    closeButton->SetPosition(GetPosition() + glm::vec2(size.x - closeButton->GetSize().x, 0.f));
    selectButton->SetPosition(GetPosition() + glm::vec2(size.x - selectButton->GetSize().x - 32.f, size.y - selectButton->GetSize().y - 32.f));
    
    for (Widget* file : fileList->GetWidgets())
        file->SetSize(glm::vec2(size.x - 20.f, 64.f));
    
    fileList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    fileList->SetSize(glm::vec2(size.x, selectButton->GetPosition().y - closeButton->GetSize().y - 64.f));
}

void FileSelector::SetClosedCallback(std::function<void(const std::string&)> callback) {
    closedCallback = callback;
    hasClosedCallback = true;
}

void FileSelector::Close() {
    shouldClose = true;
}

void FileSelector::Select() {
    shouldClose = true;
}

void FileSelector::OpenParentDirectory() {
    path = FileSystem::GetParentDirectory(path);
    pathChanged = true;
}

void FileSelector::OpenDirectory(const string& name) {
    path += FileSystem::DELIMITER + name;
    pathChanged = true;
}

void FileSelector::ScanDirectory() {
    fileList->ClearWidgets();
    
    // ..
    TextButton* parentButton = new TextButton(this, font, "..");
    parentButton->SetClickedCallback(std::bind(&OpenParentDirectory, this));
    parentButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
    fileList->AddWidget(parentButton);
    
    // Directories.
    vector<string> files = FileSystem::DirectoryContents(path, FileSystem::DIRECTORY);
    for (string file : files) {
        TextButton* fileButton = new TextButton(this, font, file);
        fileButton->SetClickedCallback(std::bind(&OpenDirectory, this, file));
        fileButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
        fileList->AddWidget(fileButton);
    }
    
    /// @todo Files.
}
