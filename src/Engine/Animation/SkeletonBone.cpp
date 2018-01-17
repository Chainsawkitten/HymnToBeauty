#include "SkeletonBone.hpp"

using namespace Animation;

void SkeletonBone::Save(std::ofstream* file) {
    file->write(reinterpret_cast<char*>(&localTx), sizeof(float) * 16);
    file->write(reinterpret_cast<char*>(&globalTx), sizeof(float) * 16);
    file->write(reinterpret_cast<char*>(&inversed), sizeof(float) * 16);
    file->write(reinterpret_cast<char*>(&parentId), sizeof(uint32_t));
}

void SkeletonBone::Load(std::ifstream* file) {
    file->read(reinterpret_cast<char*>(&localTx), sizeof(float) * 16);
    file->read(reinterpret_cast<char*>(&globalTx), sizeof(float) * 16);
    file->read(reinterpret_cast<char*>(&inversed), sizeof(float) * 16);
    file->read(reinterpret_cast<char*>(&parentId), sizeof(uint32_t));
}
