#include "Animation.hpp"
#include "Bone.hpp"

Animation::Animation::~Animation() {
    if (bones)
        delete[] bones;

    if (rootPositionKeys)
        delete[] rootPositionKeys;

    if (rootPositions)
        delete[] rootPositions;
}

void Animation::Animation::Save(std::ofstream* file) {
    file->write(reinterpret_cast<char*>(&length), sizeof(int32_t));
    file->write(reinterpret_cast<char*>(&numBones), sizeof(uint32_t));
    file->write(reinterpret_cast<char*>(&numRootPositions), sizeof(int32_t));

    file->write(reinterpret_cast<char*>(rootPositionKeys), sizeof(int32_t) * numRootPositions);
    file->write(reinterpret_cast<char*>(rootPositions), sizeof(glm::vec3) * numRootPositions);

    for (unsigned int i = 0; i < numBones; ++i)
        bones[i].Save(file);
}

void Animation::Animation::Load(std::ifstream* file) {
    file->read(reinterpret_cast<char*>(&length), sizeof(int32_t));
    file->read(reinterpret_cast<char*>(&numBones), sizeof(uint32_t));
    file->read(reinterpret_cast<char*>(&numRootPositions), sizeof(int32_t));

    if (bones)
        delete[] bones;

    if (rootPositionKeys)
        delete[] rootPositionKeys;

    if (rootPositions)
        delete[] rootPositions;

    rootPositionKeys = new int32_t[numRootPositions];
    file->read(reinterpret_cast<char*>(rootPositionKeys), sizeof(int32_t) * numRootPositions);

    rootPositions = new glm::vec3[numRootPositions];
    file->read(reinterpret_cast<char*>(rootPositions), sizeof(glm::vec3) * numRootPositions);

    bones = new Bone[numBones];
    for (unsigned int i = 0; i < numBones; ++i)
        bones[i].Load(file);
}
