#include "IntEditor.hpp"

#include <exception>

using namespace GUI;

IntEditor::IntEditor(Widget* parent, Font* font) : TextField(parent, font) {
    
}

void IntEditor::SetInt(int* variable) {
    this->variable = variable;
    
    SetText(std::to_string(*variable));
}

void IntEditor::TextUpdated() {
    try {
        *variable = std::stoi(GetText());
    } catch (std::exception& e) { }
}
