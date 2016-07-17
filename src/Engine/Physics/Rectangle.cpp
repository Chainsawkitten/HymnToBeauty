#include "Rectangle.hpp"

using namespace Physics;

Rectangle::Rectangle(const glm::vec2& position, const glm::vec2& size) {
    this->position = position;
    this->size = size;
}

bool Rectangle::Collide(const glm::vec2& point) {
    return point.x >= position.x && point.x < position.x + size.x &&
           point.y >= position.y && point.y < position.y + size.y;
}
