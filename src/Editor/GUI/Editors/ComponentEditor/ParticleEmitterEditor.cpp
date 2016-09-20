#include "ParticleEmitterEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/ParticleEmitter.hpp>
#include "../BoolEditor.hpp"
#include "../FloatEditor.hpp"
#include "../DoubleEditor.hpp"
#include "../IntEditor.hpp"
#include "../Vec2Editor.hpp"
#include "../Vec3Editor.hpp"

using namespace GUI;

ParticleEmitterEditor::ParticleEmitterEditor(Widget* parent) : ComponentEditor(parent, "Particle emitter") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::ParticleEmitter>();
    
    textureIndexEditor = new IntEditor(this, font);
    AddEditor("Texture index", textureIndexEditor);
    
    minVelocityEditor = new Vec3Editor(this, font);
    AddEditor("Min velocity", minVelocityEditor);
    
    maxVelocityEditor = new Vec3Editor(this, font);
    AddEditor("Max velocity", maxVelocityEditor);
    
    minLifetimeEditor = new FloatEditor(this, font);
    AddEditor("Min lifetime", minLifetimeEditor);
    
    maxLifetimeEditor = new FloatEditor(this, font);
    AddEditor("Max lifetime", maxLifetimeEditor);
    
    minSizeEditor = new Vec2Editor(this, font);
    AddEditor("Min size", minSizeEditor);
    
    maxSizeEditor = new Vec2Editor(this, font);
    AddEditor("Max size", maxSizeEditor);
    
    uniformScalingEditor = new BoolEditor(this);
    AddEditor("Uniform scaling", uniformScalingEditor);
    
    startAlphaEditor = new FloatEditor(this, font);
    AddEditor("Start alpha", startAlphaEditor);
    
    midAlphaEditor = new FloatEditor(this, font);
    AddEditor("Mid alpha", midAlphaEditor);
    
    endAlphaEditor = new FloatEditor(this, font);
    AddEditor("End alpha", endAlphaEditor);
    
    colorEditor = new Vec3Editor(this, font);
    AddEditor("Color", colorEditor);
    
    sizeEditor = new Vec3Editor(this, font);
    AddEditor("Size", sizeEditor);
    
    minEmitTimeEditor = new DoubleEditor(this, font);
    AddEditor("Min emit time", minEmitTimeEditor);
    
    maxEmitTimeEditor = new DoubleEditor(this, font);
    AddEditor("Max emit time", maxEmitTimeEditor);
    
    //emitterTypeEditor = new IntEditor(this, font);
    //AddEditor("Emitter type", emitterTypeEditor);
    
    SetVisible(false);
}

ParticleEmitterEditor::~ParticleEmitterEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete textureIndexEditor;
    delete minVelocityEditor;
    delete maxVelocityEditor;
    delete minLifetimeEditor;
    delete maxLifetimeEditor;
    delete minSizeEditor;
    delete maxSizeEditor;
    delete uniformScalingEditor;
    delete startAlphaEditor;
    delete midAlphaEditor;
    delete endAlphaEditor;
    delete colorEditor;
    delete sizeEditor;
    delete minEmitTimeEditor;
    delete maxEmitTimeEditor;
    //delete emitterTypeEditor;
}

void ParticleEmitterEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::ParticleEmitter* particleEmitter = entity->GetComponent<Component::ParticleEmitter>();
        SetVisible(particleEmitter != nullptr);
        
        if (IsVisible()) {
            textureIndexEditor->SetInt(&particleEmitter->particleType.textureIndex);
            minVelocityEditor->SetVec3(&particleEmitter->particleType.minVelocity);
            maxVelocityEditor->SetVec3(&particleEmitter->particleType.maxVelocity);
            minLifetimeEditor->SetFloat(&particleEmitter->particleType.minLifetime);
            maxLifetimeEditor->SetFloat(&particleEmitter->particleType.maxLifetime);
            minSizeEditor->SetVec2(&particleEmitter->particleType.minSize);
            maxSizeEditor->SetVec2(&particleEmitter->particleType.maxSize);
            uniformScalingEditor->SetBool(&particleEmitter->particleType.uniformScaling);
            startAlphaEditor->SetFloat(&particleEmitter->particleType.startAlpha);
            midAlphaEditor->SetFloat(&particleEmitter->particleType.midAlpha);
            endAlphaEditor->SetFloat(&particleEmitter->particleType.endAlpha);
            colorEditor->SetVec3(&particleEmitter->particleType.color);
            sizeEditor->SetVec3(&particleEmitter->size);
            minEmitTimeEditor->SetDouble(&particleEmitter->minEmitTime);
            maxEmitTimeEditor->SetDouble(&particleEmitter->maxEmitTime);
            //emitterTypeEditor->SetInt(static_cast<int*>(&particleEmitter->emitterType));
        }
    }
}
