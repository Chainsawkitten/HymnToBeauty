#include "DoubleEditor.hpp"

#include <exception>

using namespace GUI;

DoubleEditor::DoubleEditor(Widget* parent, Font* font) : TextField(parent, font) {
    
}

void DoubleEditor::SetDouble(double* variable) {
    this->variable = variable;
    
    SetText(std::to_string(*variable));
}

void DoubleEditor::TextUpdated() {
    try {
        *variable = std::stod(GetText());
    } catch (std::exception& e) { }
}
