#include "MeshEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"
#include "Label.hpp"

using namespace GUI;

MeshEditor::MeshEditor(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    SetVisible(false);
    mesh = nullptr;
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    nameLabel = new Label(this, font, "Name");
}

MeshEditor::~MeshEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete nameLabel;
}

void MeshEditor::Update() {
    
}

void MeshEditor::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    nameLabel->Render(screenSize);
}

void MeshEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    nameLabel->SetPosition(position);
}

glm::vec2 MeshEditor::GetSize() const {
    return size;
}

void MeshEditor::SetSize(const glm::vec2& size) {
    this->size = size;
}

void MeshEditor::SetMesh(Geometry::OBJModel* mesh) {
    this->mesh = mesh;
}
