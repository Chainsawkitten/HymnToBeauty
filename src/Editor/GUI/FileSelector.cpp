#include "FileSelector.hpp"

#include "ImageButton.hpp"
#include "ImageTextButton.hpp"
#include "VerticalScrollLayout.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <Directory.png.hpp>
#include <BasicFile.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Util/FileSystem.hpp>

using namespace GUI;
using namespace std;

FileSelector::FileSelector(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    directoryTexture = Managers().resourceManager->CreateTexture2D(DIRECTORY_PNG, DIRECTORY_PNG_LENGTH);
    fileTexture = Managers().resourceManager->CreateTexture2D(BASICFILE_PNG, BASICFILE_PNG_LENGTH);
    fileList = new VerticalScrollLayout(this);
    AddWidget(fileList);
    
    path = FileSystem::DataPath("Hymn to Beauty");
    pathChanged = true;
    
    SetVisible(false);
}

FileSelector::~FileSelector() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    
    fileList->ClearWidgets();
    delete fileList;
    
    Managers().resourceManager->FreeTexture2D(directoryTexture);
    Managers().resourceManager->FreeTexture2D(fileTexture);
}

void FileSelector::Update() {
    if (pathChanged) {
        ScanDirectory();
        pathChanged = false;
    }
    
    UpdateWidgets();
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
    
    for (Widget* file : fileList->GetWidgets())
        file->SetSize(glm::vec2(size.x - 20.f, 64.f));
    
    fileList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    fileList->SetSize(glm::vec2(size.x, size.y - closeButton->GetSize().y));
}

void FileSelector::SetFileSelectedCallback(std::function<void(const std::string&)> callback) {
    fileSelectedCallback = callback;
    hasFileSelectedCallback = true;
}

void FileSelector::SetExtension(const string& extension) {
    this->extension = extension;
    ScanDirectory();
}

void FileSelector::Close() {
    SetVisible(false);
}

void FileSelector::OpenParentDirectory() {
    path = FileSystem::GetParentDirectory(path);
    pathChanged = true;
}

void FileSelector::OpenDirectory(const string& name) {
    path += FileSystem::DELIMITER + name;
    pathChanged = true;
}

void FileSelector::SelectFile(const string& name) {
    SetVisible(false);
    
    if (hasFileSelectedCallback)
        fileSelectedCallback(path + FileSystem::DELIMITER + name);
}

void FileSelector::ScanDirectory() {
    fileList->ClearWidgets();
    
    // ..
    ImageTextButton* parentButton = new ImageTextButton(this, directoryTexture, font, "..");
    parentButton->SetClickedCallback(std::bind(&OpenParentDirectory, this));
    parentButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
    fileList->AddWidget(parentButton);
    
    // Directories.
    vector<string> files = FileSystem::DirectoryContents(path, FileSystem::DIRECTORY);
    for (string file : files) {
        ImageTextButton* fileButton = new ImageTextButton(this, directoryTexture, font, file);
        fileButton->SetClickedCallback(std::bind(&OpenDirectory, this, file));
        fileButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
        fileList->AddWidget(fileButton);
    }
    
    // Files.
    files = FileSystem::DirectoryContents(path, FileSystem::FILE);
    for (string file : files) {
        if (FileSystem::GetExtension(file) == extension) {
            ImageTextButton* fileButton = new ImageTextButton(this, fileTexture, font, file);
            fileButton->SetClickedCallback(std::bind(&SelectFile, this, file));
            fileButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
            fileList->AddWidget(fileButton);
        }
    }
}
