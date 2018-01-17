#include "Bone.hpp"

using namespace Animation;

Bone::~Bone() {
    if (rotationKeys != nullptr)
        delete[] rotationKeys;

    if (rotations != nullptr)
        delete[] rotations;
}

void Bone::Save(std::ofstream* file) {
    file->write(reinterpret_cast<char*>(&parent), sizeof(uint32_t));
    file->write(reinterpret_cast<char*>(&numRotationKeys), sizeof(uint32_t));

    for (unsigned int i = 0; i < numRotationKeys; ++i)
        file->write(reinterpret_cast<char*>(&rotationKeys[i]), sizeof(int32_t));

    for (unsigned int i = 0; i < numRotationKeys; ++i)
        file->write(reinterpret_cast<char*>(&rotations[i]), sizeof(glm::quat));
}

void Bone::Load(std::ifstream* file) {
    file->read(reinterpret_cast<char*>(&parent), sizeof(uint32_t));
    file->read(reinterpret_cast<char*>(&numRotationKeys), sizeof(uint32_t));

    if (rotationKeys != nullptr)
        delete[] rotationKeys;

    if (rotations != nullptr)
        delete[] rotations;

    rotationKeys = new int32_t[numRotationKeys];
    for (unsigned int i = 0; i < numRotationKeys; ++i)
        file->read(reinterpret_cast<char*>(&rotationKeys[i]), sizeof(int32_t));

    rotations = new glm::quat[numRotationKeys];
    for (unsigned int i = 0; i < numRotationKeys; ++i)
        file->read(reinterpret_cast<char*>(&rotations[i]), sizeof(glm::quat));
}
