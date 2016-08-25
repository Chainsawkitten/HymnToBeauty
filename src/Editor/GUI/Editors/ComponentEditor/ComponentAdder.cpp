#include "ComponentAdder.hpp"

#include "../../ImageButton.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Close.png.hpp>
#include <ABeeZee.ttf.hpp>

#include <Engine/Component/Lens.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Transform.hpp>
#include <Engine/Component/Physics.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/PointLight.hpp>
#include <Engine/Component/SpotLight.hpp>
#include <Engine/Component/Listener.hpp>
#include <Engine/Component/SoundSource.hpp>

using namespace GUI;

ComponentAdder::ComponentAdder(Widget *parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    closeTexture = Managers().resourceManager->CreateTexture2D(CLOSE_PNG, CLOSE_PNG_LENGTH);
    closeButton = new ImageButton(this, closeTexture);
    closeButton->SetClickedCallback(std::bind(&Close, this));
    AddWidget(closeButton);
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 24.f);
    
    componentList = new VerticalScrollLayout(this);
    AddWidget(componentList);
    
    SetVisible(false);
}

ComponentAdder::~ComponentAdder() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete closeButton;
    Managers().resourceManager->FreeTexture2D(closeTexture);
    
    componentList->ClearWidgets();
    delete componentList;
}

void ComponentAdder::Update() {
    UpdateWidgets();
}

void ComponentAdder::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), GetSize(), color);
    
    RenderWidgets();
}

glm::vec2 ComponentAdder::GetSize() const {
    return size;
}

void ComponentAdder::SetSize(const glm::vec2& size) {
    this->size = size;
    
    closeButton->SetPosition(GetPosition() + glm::vec2(size.x - closeButton->GetSize().x, 0.f));
    
    for (Widget* component : componentList->GetWidgets())
        component->SetSize(glm::vec2(size.x - 20.f, 64.f));
    
    componentList->SetPosition(GetPosition() + glm::vec2(0.f, closeButton->GetSize().y));
    componentList->SetSize(glm::vec2(size.x, size.y - closeButton->GetSize().y));
}

void ComponentAdder::SetEntity(Entity* entity) {
    this->entity = entity;
    
    componentList->ClearWidgets();
    
    AddComponentButton<Component::Transform>("Transform");
    AddComponentButton<Component::Physics>("Physics");
    AddComponentButton<Component::Mesh>("Mesh");
    AddComponentButton<Component::Lens>("Lens");
    AddComponentButton<Component::Material>("Material");
    AddComponentButton<Component::DirectionalLight>("Directional light");
    AddComponentButton<Component::PointLight>("Point light");
    AddComponentButton<Component::SpotLight>("Spot light");
    AddComponentButton<Component::Listener>("Listener");
    AddComponentButton<Component::SoundSource>("SoundSource");
}

void ComponentAdder::SetComponentAddedCallback(std::function<void()> callback) {
    this->componentAddedCallback = callback;
    hasComponentAddedCallback = true;
}

void ComponentAdder::Close() {
    SetVisible(false);
}
