#include "ResourceList.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>

using namespace GUI;

ResourceList::ResourceList(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
}

ResourceList::~ResourceList() {
    Managers().resourceManager->FreeRectangle();
}

void ResourceList::Update() {
    
}

void ResourceList::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
}

glm::vec2 ResourceList::GetSize() const {
    return size;
}

void ResourceList::SetSize(const glm::vec2& size) {
    this->size = size;
}
