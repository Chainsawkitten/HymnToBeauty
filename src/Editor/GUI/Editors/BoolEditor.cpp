#include "BoolEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "Unchecked.png.hpp"
#include "Checked.png.hpp"
#include <Engine/Util/Input.hpp>
#include <Engine/Physics/Rectangle.hpp>

using namespace GUI;

BoolEditor::BoolEditor(Widget* parent) : Widget(parent) {
    uncheckedTexture = Managers().resourceManager->CreateTexture2D(UNCHECKED_PNG, UNCHECKED_PNG_LENGTH, false);
    checkedTexture = Managers().resourceManager->CreateTexture2D(CHECKED_PNG, CHECKED_PNG_LENGTH, false);
}

BoolEditor::~BoolEditor() {
    Managers().resourceManager->FreeTexture2D(uncheckedTexture);
    Managers().resourceManager->FreeTexture2D(checkedTexture);
}

void BoolEditor::SetBool(bool* variable) {
    this->variable = variable;
}

void BoolEditor::Update() {
    if (Input()->Triggered(InputHandler::CLICK)) {
        Physics::Rectangle rect(GetPosition(), GetSize());
        if (rect.Collide(glm::vec2(Input()->CursorX(), Input()->CursorY())))
            *variable = !*variable;
    }
}

void BoolEditor::Render() {
    if (*variable)
        checkedTexture->Render(GetPosition(), GetSize());
    else
        uncheckedTexture->Render(GetPosition(), GetSize());
}

glm::vec2 BoolEditor::GetSize() const {
    return glm::vec2(checkedTexture->GetWidth(), checkedTexture->GetHeight());
}

void BoolEditor::SetSize(const glm::vec2& size) {
    // Do nothing.
}
