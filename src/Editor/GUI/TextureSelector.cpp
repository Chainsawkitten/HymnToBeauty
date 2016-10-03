#include "TextureSelector.hpp"

#include "ImageButton.hpp"
#include "TextButton.hpp"
#include "VerticalScrollLayout.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Texture/Texture2D.hpp>

using namespace GUI;
using namespace std;

TextureSelector::TextureSelector(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&TextureSelector::Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    textureList = new VerticalScrollLayout(this);
    AddWidget(textureList);
    
    SetVisible(false);
}

TextureSelector::~TextureSelector() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    
    textureList->ClearWidgets();
    delete textureList;
}

void TextureSelector::Update() {
    UpdateWidgets();
}

void TextureSelector::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), GetSize(), color);
    
    RenderWidgets();
}

glm::vec2 TextureSelector::GetSize() const {
    return size;
}

void TextureSelector::SetSize(const glm::vec2& size) {
    this->size = size;
    
    closeButton->SetPosition(GetPosition() + glm::vec2(size.x - closeButton->GetSize().x, 0.f));
    
    for (Widget* texture : textureList->GetWidgets())
        texture->SetSize(glm::vec2(size.x - 20.f, 64.f));
    
    textureList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    textureList->SetSize(glm::vec2(size.x, size.y - closeButton->GetSize().y));
}

void TextureSelector::SetTextureSelectedCallback(std::function<void(Texture2D*)> callback) {
    textureSelectedCallback = callback;
    hasTextureSelectedCallback = true;
}

void TextureSelector::SetTextures(const std::vector<Texture2D*>* textures) {
    this->textures = textures;
    UpdateTextures();
}

void TextureSelector::UpdateTextures() {
    textureList->ClearWidgets();
    
    for (Texture2D* texture : *textures) {
        TextButton* textureButton = new TextButton(this, font, texture->name);
        textureButton->SetClickedCallback(std::bind(&TextureSelector::TextureSelected, this, texture));
        textureButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
        textureList->AddWidget(textureButton);
    }
}

void TextureSelector::Close() {
    SetVisible(false);
}

void TextureSelector::TextureSelected(Texture2D* texture) {
    SetVisible(false);
    
    if (hasTextureSelectedCallback)
        textureSelectedCallback(texture);
}
