#include "ComponentEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include "ABeeZee.ttf.hpp"
#include "../Label.hpp"

using namespace GUI;

ComponentEditor::ComponentEditor(Widget* parent, const std::string& title) : Widget(parent) {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    titleLabel = new Label(this, font, title);
}

ComponentEditor::~ComponentEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete titleLabel;
    
    for (LabeledEditor& editor : editors)
        delete editor.label;
}

void ComponentEditor::Update() {
    if (IsVisible()) {
        for (LabeledEditor& editor : editors) {
            editor.editor->Update();
        }
    }
}

void ComponentEditor::Render(const glm::vec2& screenSize) {
    if (IsVisible()) {
        titleLabel->Render(screenSize);
        
        for (LabeledEditor& editor : editors) {
            editor.label->Render(screenSize);
            editor.editor->Render(screenSize);
        }
    }
}

void ComponentEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    glm::vec2 pos(position);
    
    titleLabel->SetPosition(pos);
    pos += glm::vec2(10.f, 20.f);
    
    for (LabeledEditor& editor : editors) {
        editor.label->SetPosition(pos);
        pos.y += 20.f;
        editor.editor->SetPosition(pos + glm::vec2(10.f, 0.f));
        pos.y += editor.editor->GetSize().y + 5.f;
    }
}

glm::vec2 ComponentEditor::GetSize() const {
    return size;
}

void ComponentEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    this->size.y = 25.f;
    
    for (LabeledEditor& editor : editors) {
        editor.editor->SetSize(glm::vec2(size.x - 20.f, 20.f));
        this->size.y += 20.f + editor.editor->GetSize().y;
    }
}

void ComponentEditor::AddEditor(const std::string& name, Widget* editor) {
    editors.push_back(LabeledEditor(new Label(this, font, name), editor));
}

ComponentEditor::LabeledEditor::LabeledEditor(Label* label, Widget* editor) {
    this->label = label;
    this->editor = editor;
}
