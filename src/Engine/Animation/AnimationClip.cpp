#include "AnimationClip.hpp"
#include <fstream>
#include "../Hymn.hpp"
#include <Utility/Log.hpp>
#include "Animation.hpp"

using namespace Animation;

void AnimationClip::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);

    std::ifstream file(Hymn().GetPath() + "/" + name + ".asset", std::ios::binary);
    if (!file.is_open()) {
        Log() << "Could not open animation file: " << Hymn().GetPath() + "/" + name + ".asset" << "\n";
        file.close();
        return;
    }

    animation = new Animation;
    animation->Load(&file);
    file.close();
}
