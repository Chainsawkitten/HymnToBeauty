#include "StringEditor.hpp"

#include <exception>

using namespace GUI;

StringEditor::StringEditor(Widget* parent, Font* font) : TextField(parent, font) {
    variable = nullptr;
}

void StringEditor::SetString(std::string* variable) {
    this->variable = variable;
    
    SetText(*variable);
}

void StringEditor::TextUpdated() {
    *variable = GetText();
}
