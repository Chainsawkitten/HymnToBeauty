#include "ModelEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Geometry/OBJModel.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"
#include <Engine/Texture/Texture2D.hpp>
#include "Subtract.png.hpp"
#include "../Label.hpp"
#include "StringEditor.hpp"
#include "../TextButton.hpp"
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include "../ImageTextButton.hpp"

using namespace GUI;

ModelEditor::ModelEditor(Widget* parent, FileSelector* fileSelector) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    SetVisible(false);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    nameLabel = new Label(this, font, "Name");
    nameEditor = new StringEditor(this, font);
    
    deleteModelTexture = Managers().resourceManager->CreateTexture2D(SUBTRACT_PNG, SUBTRACT_PNG_LENGTH);
    deleteModelButton = new ImageTextButton(this, deleteModelTexture, font, "Delete model");
    deleteModelButton->SetImageSize(glm::vec2(deleteModelTexture->GetWidth(), deleteModelTexture->GetHeight()));
    deleteModelButton->SetClickedCallback(std::bind(&DeleteModelPressed, this));
    
    loadButton = new TextButton(this, font, "Load OBJ model");
    loadButton->SetClickedCallback(std::bind(&LoadPressed, this));
    this->fileSelector = fileSelector;
}

ModelEditor::~ModelEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    Managers().resourceManager->FreeTexture2D(deleteModelTexture);
    delete deleteModelButton;
    
    delete nameLabel;
    delete nameEditor;
    delete loadButton;
}

void ModelEditor::Update() {
    nameEditor->Update();
    deleteModelButton->Update();
    loadButton->Update();
}

void ModelEditor::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color);
    
    nameLabel->Render();
    nameEditor->Render();
    deleteModelButton->Render();
    loadButton->Render();
}

void ModelEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    nameLabel->SetPosition(position);
    nameEditor->SetPosition(position + glm::vec2(10.f, 20.f));
    deleteModelButton->SetPosition(position + glm::vec2(0.f, 50.f));
    loadButton->SetPosition(position + glm::vec2(0.f, 70.f));
}

glm::vec2 ModelEditor::GetSize() const {
    return size;
}

void ModelEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    nameEditor->SetSize(glm::vec2(size.x - 10.f, 20.f));
    deleteModelButton->SetSize(glm::vec2(size.x, 20.f));
    loadButton->SetSize(glm::vec2(size.x, 20.f));
}

void ModelEditor::SetModel(Geometry::OBJModel* model) {
    this->model = model;
    
    nameEditor->SetString(&model->name);
    
    // Update editor positions.
    SetPosition(GetPosition());
}

void ModelEditor::DeleteModelPressed() {
    delete model;
    for (auto it = Hymn().models.begin(); it != Hymn().models.end(); ++it) {
        if (*it == model) {
            Hymn().models.erase(it);
            break;
        }
    }
    SetVisible(false);
}

void ModelEditor::LoadPressed() {
    fileSelector->SetExtension("obj");
    fileSelector->SetFileSelectedCallback(std::bind(&FileSelected, this, std::placeholders::_1));
    fileSelector->SetVisible(true);
}

void ModelEditor::FileSelected(const std::string& file) {
    std::string destination = Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + model->name + ".obj";
    FileSystem::Copy(file.c_str(), destination.c_str());
    model->Load(file.c_str());
}
