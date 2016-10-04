#include "ModelSelector.hpp"

#include "ImageButton.hpp"
#include "TextButton.hpp"
#include "VerticalScrollLayout.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Geometry/Model.hpp>

using namespace GUI;
using namespace std;

ModelSelector::ModelSelector(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&ModelSelector::Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    modelList = new VerticalScrollLayout(this);
    AddWidget(modelList);
    
    SetVisible(false);
}

ModelSelector::~ModelSelector() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    
    modelList->ClearWidgets();
    delete modelList;
}

void ModelSelector::Update() {
    UpdateWidgets();
}

void ModelSelector::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), GetSize(), color);
    
    RenderWidgets();
}

glm::vec2 ModelSelector::GetSize() const {
    return size;
}

void ModelSelector::SetSize(const glm::vec2& size) {
    this->size = size;
    
    closeButton->SetPosition(GetPosition() + glm::vec2(size.x - closeButton->GetSize().x, 0.f));
    
    for (Widget* model : modelList->GetWidgets())
        model->SetSize(glm::vec2(size.x - 20.f, 64.f));
    
    modelList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    modelList->SetSize(glm::vec2(size.x, size.y - closeButton->GetSize().y));
}

void ModelSelector::SetModelSelectedCallback(std::function<void(Geometry::Model*)> callback) {
    modelSelectedCallback = callback;
    hasModelSelectedCallback = true;
}

void ModelSelector::SetModels(const std::vector<Geometry::Model*>* models) {
    this->models = models;
    UpdateModels();
}

void ModelSelector::UpdateModels() {
    modelList->ClearWidgets();
    
    for (Geometry::Model* model : *models) {
        TextButton* modelButton = new TextButton(this, font, model->name);
        modelButton->SetClickedCallback(std::bind(&ModelSelector::ModelSelected, this, model));
        modelButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
        modelList->AddWidget(modelButton);
    }
}

void ModelSelector::Close() {
    SetVisible(false);
}

void ModelSelector::ModelSelected(Geometry::Model* model) {
    SetVisible(false);
    
    if (hasModelSelectedCallback)
        modelSelectedCallback(model);
}
