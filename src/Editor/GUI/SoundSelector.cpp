#include "SoundSelector.hpp"

#include "ImageButton.hpp"
#include "TextButton.hpp"
#include "VerticalScrollLayout.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <ABeeZee.ttf.hpp>
#include <Engine/Audio/SoundBuffer.hpp>

using namespace GUI;
using namespace std;

SoundSelector::SoundSelector(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&SoundSelector::Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    soundList = new VerticalScrollLayout(this);
    AddWidget(soundList);
    
    SetVisible(false);
}

SoundSelector::~SoundSelector() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    
    soundList->ClearWidgets();
    delete soundList;
}

void SoundSelector::Update() {
    UpdateWidgets();
}

void SoundSelector::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), GetSize(), color);
    
    RenderWidgets();
}

glm::vec2 SoundSelector::GetSize() const {
    return size;
}

void SoundSelector::SetSize(const glm::vec2& size) {
    this->size = size;
    
    closeButton->SetPosition(GetPosition() + glm::vec2(size.x - closeButton->GetSize().x, 0.f));
    
    for (Widget* sound : soundList->GetWidgets())
        sound->SetSize(glm::vec2(size.x - 20.f, 64.f));
    
    soundList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    soundList->SetSize(glm::vec2(size.x, size.y - closeButton->GetSize().y));
}

void SoundSelector::SetSoundSelectedCallback(std::function<void(Audio::SoundBuffer*)> callback) {
    soundSelectedCallback = callback;
    hasSoundSelectedCallback = true;
}

void SoundSelector::SetSounds(const std::vector<Audio::SoundBuffer*>* sounds) {
    this->sounds = sounds;
    UpdateSounds();
}

void SoundSelector::UpdateSounds() {
    soundList->ClearWidgets();
    
    for (Audio::SoundBuffer* sound : *sounds) {
        TextButton* soundButton = new TextButton(this, font, sound->name);
        soundButton->SetClickedCallback(std::bind(&SoundSelector::SoundSelected, this, sound));
        soundButton->SetSize(glm::vec2(size.x - 20.f, 64.f));
        soundList->AddWidget(soundButton);
    }
}

void SoundSelector::Close() {
    SetVisible(false);
}

void SoundSelector::SoundSelected(Audio::SoundBuffer* sound) {
    SetVisible(false);
    
    if (hasSoundSelectedCallback)
        soundSelectedCallback(sound);
}
