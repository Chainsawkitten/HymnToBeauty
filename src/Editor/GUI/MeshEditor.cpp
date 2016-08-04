#include "MeshEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Geometry/OBJModel.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"
#include "Label.hpp"
#include "StringEditor.hpp"
#include "TextButton.hpp"
#include "FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>

using namespace GUI;

MeshEditor::MeshEditor(Widget* parent, FileSelector* fileSelector) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    SetVisible(false);
    mesh = nullptr;
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    nameLabel = new Label(this, font, "Name");
    nameEditor = new StringEditor(this, font);
    
    loadButton = new TextButton(this, font, "Load OBJ model");
    loadButton->SetClickedCallback(std::bind(&LoadPressed, this));
    this->fileSelector = fileSelector;
}

MeshEditor::~MeshEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete nameLabel;
    delete nameEditor;
    delete loadButton;
}

void MeshEditor::Update() {
    nameEditor->Update();
    loadButton->Update();
}

void MeshEditor::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    nameLabel->Render(screenSize);
    nameEditor->Render(screenSize);
    loadButton->Render(screenSize);
}

void MeshEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    nameLabel->SetPosition(position);
    nameEditor->SetPosition(position + glm::vec2(10.f, 20.f));
    loadButton->SetPosition(position + glm::vec2(0.f, 50.f));
}

glm::vec2 MeshEditor::GetSize() const {
    return size;
}

void MeshEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    nameEditor->SetSize(glm::vec2(size.x - 10.f, 20.f));
    loadButton->SetSize(glm::vec2(size.x, 20.f));
}

void MeshEditor::SetMesh(Geometry::OBJModel* mesh) {
    this->mesh = mesh;
    
    nameEditor->SetString(&mesh->name);
    
    // Update editor positions.
    SetPosition(GetPosition());
}

void MeshEditor::LoadPressed() {
    fileSelector->SetExtension("obj");
    fileSelector->SetFileSelectedCallback(std::bind(&FileSelected, this, std::placeholders::_1));
    fileSelector->SetVisible(true);
}

void MeshEditor::FileSelected(const std::string& file) {
    std::string destination = Hymn().GetPath() + FileSystem::DELIMITER + mesh->name + ".obj";
    FileSystem::Copy(file.c_str(), destination.c_str());
    mesh->Load(file.c_str());
}
