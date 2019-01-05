#include "Node.hpp"

Node::~Node() {}

void Node::Save(std::ofstream* file) {
    file->write(reinterpret_cast<char*>(&index), sizeof(uint32_t));
    file->write(reinterpret_cast<char*>(name), 128);
    file->write(reinterpret_cast<char*>(&numInputSlots), sizeof(uint32_t));
    file->write(reinterpret_cast<char*>(inputIndex), sizeof(uint32_t) * numInputSlots);
    file->write(reinterpret_cast<char*>(&numOutputSlots), sizeof(uint32_t));
    file->write(reinterpret_cast<char*>(outputIndex), sizeof(uint32_t) * numOutputSlots);
    file->write(reinterpret_cast<char*>(&pos), sizeof(glm::vec2));
    file->write(reinterpret_cast<char*>(&size), sizeof(glm::vec2));
}

void Node::Load(std::ifstream* file) {
    file->read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
    file->read(reinterpret_cast<char*>(name), 128);
    file->read(reinterpret_cast<char*>(&numInputSlots), sizeof(uint32_t));
    file->read(reinterpret_cast<char*>(inputIndex), sizeof(uint32_t) * numInputSlots);
    file->read(reinterpret_cast<char*>(&numOutputSlots), sizeof(uint32_t));
    file->read(reinterpret_cast<char*>(outputIndex), sizeof(uint32_t) * numOutputSlots);
    file->read(reinterpret_cast<char*>(&pos), sizeof(glm::vec2));
    file->read(reinterpret_cast<char*>(&size), sizeof(glm::vec2));
}
