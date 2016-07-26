#include "FloatEditor.hpp"

using namespace GUI;

FloatEditor::FloatEditor(Widget* parent, Font* font) : TextField(parent, font) {
    variable = nullptr;
}

void FloatEditor::SetFloat(float* variable) {
    this->variable = variable;
    
    SetText(std::to_string(*variable));
}
