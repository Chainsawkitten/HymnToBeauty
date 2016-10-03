#include "SoundSourceEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/SoundSource.hpp>
#include "../FloatEditor.hpp"
#include "../BoolEditor.hpp"
#include "../../TextButton.hpp"
#include "../../SoundSelector.hpp"

using namespace GUI;

SoundSourceEditor::SoundSourceEditor(Widget* parent, SoundSelector* soundSelector) : ComponentEditor(parent, "Sound source") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::SoundSource>();
    
    selectSoundButton = new TextButton(this, font, "Select sound");
    selectSoundButton->SetClickedCallback(std::bind(&SoundSourceEditor::SelectSoundPressed, this));
    AddEditor("Sound", selectSoundButton);
    
    pitchEditor = new FloatEditor(this, font);
    AddEditor("Pitch", pitchEditor);
    
    gainEditor = new FloatEditor(this, font);
    AddEditor("Gain", gainEditor);
    
    loopEditor = new BoolEditor(this);
    AddEditor("Loop", loopEditor);
    
    this->soundSelector = soundSelector;
    
    SetVisible(false);
}

SoundSourceEditor::~SoundSourceEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete selectSoundButton;
    delete pitchEditor;
    delete gainEditor;
    delete loopEditor;
}

void SoundSourceEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        this->soundSource = entity->GetComponent<Component::SoundSource>();
        SetVisible(soundSource != nullptr);
        
        if (IsVisible()) {
            pitchEditor->SetFloat(&soundSource->pitch);
            gainEditor->SetFloat(&soundSource->gain);
            loopEditor->SetBool(&soundSource->loop);
        }
    }
}

void SoundSourceEditor::SelectSoundPressed() {
    soundSelector->SetVisible(true);
    soundSelector->UpdateSounds();
    soundSelector->SetSoundSelectedCallback(std::bind(&SoundSourceEditor::SoundSelected, this, std::placeholders::_1));
}

void SoundSourceEditor::SoundSelected(Audio::SoundBuffer* sound) {
    soundSource->soundBuffer = sound;
}
