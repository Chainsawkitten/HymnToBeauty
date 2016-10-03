#include "SoundEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Audio/VorbisFile.hpp>
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

SoundEditor::SoundEditor(Widget* parent, FileSelector* fileSelector) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    SetVisible(false);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    nameLabel = new Label(this, font, "Name");
    nameEditor = new StringEditor(this, font);
    
    deleteSoundTexture = Managers().resourceManager->CreateTexture2D(SUBTRACT_PNG, SUBTRACT_PNG_LENGTH);
    deleteSoundButton = new ImageTextButton(this, deleteSoundTexture, font, "Delete sound");
    deleteSoundButton->SetImageSize(glm::vec2(deleteSoundTexture->GetWidth(), deleteSoundTexture->GetHeight()));
    deleteSoundButton->SetClickedCallback(std::bind(&SoundEditor::DeleteSoundPressed, this));
    
    loadButton = new TextButton(this, font, "Load Ogg Vorbis");
    loadButton->SetClickedCallback(std::bind(&SoundEditor::LoadPressed, this));
    this->fileSelector = fileSelector;
}

SoundEditor::~SoundEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    Managers().resourceManager->FreeTexture2D(deleteSoundTexture);
    delete deleteSoundButton;
    
    delete nameLabel;
    delete nameEditor;
    delete loadButton;
}

void SoundEditor::Update() {
    nameEditor->Update();
    deleteSoundButton->Update();
    loadButton->Update();
}

void SoundEditor::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color);
    
    nameLabel->Render();
    nameEditor->Render();
    deleteSoundButton->Render();
    loadButton->Render();
}

void SoundEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    nameLabel->SetPosition(position);
    nameEditor->SetPosition(position + glm::vec2(10.f, 20.f));
    deleteSoundButton->SetPosition(position + glm::vec2(0.f, 50.f));
    loadButton->SetPosition(position + glm::vec2(0.f, 70.f));
}

glm::vec2 SoundEditor::GetSize() const {
    return size;
}

void SoundEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    nameEditor->SetSize(glm::vec2(size.x - 10.f, 20.f));
    deleteSoundButton->SetSize(glm::vec2(size.x, 20.f));
    loadButton->SetSize(glm::vec2(size.x, 20.f));
}

void SoundEditor::SetSound(Audio::SoundBuffer* sound) {
    this->sound = sound;
    
    nameEditor->SetString(&sound->name);
    
    // Update editor positions.
    SetPosition(GetPosition());
}

void SoundEditor::DeleteSoundPressed() {
    delete sound;
    for (auto it = Hymn().sounds.begin(); it != Hymn().sounds.end(); ++it) {
        if (*it == sound) {
            Hymn().sounds.erase(it);
            break;
        }
    }
    SetVisible(false);
}

void SoundEditor::LoadPressed() {
    fileSelector->SetExtension("ogg");
    fileSelector->SetFileSelectedCallback(std::bind(&SoundEditor::FileSelected, this, std::placeholders::_1));
    fileSelector->SetVisible(true);
}

void SoundEditor::FileSelected(const std::string& file) {
    std::string destination = Hymn().GetPath() + FileSystem::DELIMITER + "Sounds" + FileSystem::DELIMITER + sound->name + ".ogg";
    FileSystem::Copy(file.c_str(), destination.c_str());
    Audio::SoundFile* soundFile = new Audio::VorbisFile(file.c_str());
    sound->Load(soundFile);
    delete soundFile;
}
