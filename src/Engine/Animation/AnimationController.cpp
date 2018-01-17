#include "AnimationController.hpp"
#include "Skeleton.hpp"
#include "AnimationClip.hpp"
#include <Utility/Log.hpp>
#include "../Hymn.hpp"
#include "AnimationAction.hpp"
#include "AnimationTransition.hpp"

using namespace Animation;

AnimationController::~AnimationController() {
    Clear();
}

void AnimationController::Save(const std::string& path) {
    // Open file.
    std::ofstream file(path, std::ios::binary);

    // Check if file is open, if not log and early return.
    if (!file.is_open()) {
        Log() << "Could not save animation controller file: " << path << "\n";
        file.close();
        return;
    }

    uint32_t numNodes = static_cast<uint32_t>(animationNodes.size());
    file.write(reinterpret_cast<char*>(&numNodes), sizeof(uint32_t));

    for (Node* node : animationNodes) {
        if (typeid(*node) == typeid(AnimationAction)) {
            NodeType nodetype = ACTION;
            file.write(reinterpret_cast<char*>(&nodetype), sizeof(NodeType));
            AnimationAction* animationAction = dynamic_cast<AnimationAction*>(node);

            if (animationAction)
                animationAction->Save(&file);
        } else {
            NodeType nodetype = TRANSITION;
            file.write(reinterpret_cast<char*>(&nodetype), sizeof(NodeType));
            AnimationTransition* animationTransition = dynamic_cast<AnimationTransition*>(node);

            if (animationTransition)
                animationTransition->Save(&file);
        }
    }

    uint32_t numBools = static_cast<uint32_t>(boolMap.size());
    file.write(reinterpret_cast<char*>(&numBools), sizeof(uint32_t));

    for (BoolItem* b : boolMap)
        file.write(reinterpret_cast<char*>(b), sizeof(BoolItem));

    uint32_t numFloats = static_cast<uint32_t>(floatMap.size());
    file.write(reinterpret_cast<char*>(&numFloats), sizeof(uint32_t));

    for (FloatItem* f : floatMap)
        file.write(reinterpret_cast<char*>(f), sizeof(FloatItem));

    // Close file.
    file.close();
}

void AnimationController::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);

    // Open file.
    std::ifstream file(Hymn().GetPath() + "/" + name + ".asset", std::ios::binary);

    // Check if file is open, if not log and early return.
    if (!file.is_open()) {
        Log() << "Could not load animation controller file: " << Hymn().GetPath() + "/" + name + ".asset" << "\n";
        file.close();
        return;
    }

    // Clear if anything is loaded.
    Clear();

    uint32_t numNodes = 0;
    file.read(reinterpret_cast<char*>(&numNodes), sizeof(uint32_t));
    for (unsigned int i = 0; i < numNodes; ++i)  {
        NodeType nodeType;
        file.read(reinterpret_cast<char*>(&nodeType), sizeof(NodeType));
        if (nodeType == ACTION) {
            AnimationAction* node = new AnimationAction;
            node->Load(&file);
            animationNodes.push_back(node);
        } else {
            AnimationTransition* node = new AnimationTransition;
            node->Load(&file);
            animationNodes.push_back(node);
        }
    }

    for (std::size_t i = 0; i < boolMap.size(); ++i)
        delete boolMap[i];

    for (std::size_t i = 0; i < floatMap.size(); ++i)
        delete floatMap[i];

    uint32_t numBools;
    file.read(reinterpret_cast<char*>(&numBools), sizeof(uint32_t));
    boolMap.resize(numBools);
    for (std::size_t i = 0; i < numBools; ++i) {
        boolMap[i] = new BoolItem;
        file.read(reinterpret_cast<char*>(boolMap[i]), sizeof(BoolItem));
    }

    uint32_t numFloats;
    file.read(reinterpret_cast<char*>(&numFloats), sizeof(uint32_t));
    floatMap.resize(numFloats);
    for (std::size_t i = 0; i < numFloats; ++i) {
        floatMap[i] = new FloatItem;
        file.read(reinterpret_cast<char*>(floatMap[i]), sizeof(FloatItem));
    }

    // Close file.
    file.close();
}

void AnimationController::Clear() {
    for (Node* node : animationNodes)
        delete node;
    animationNodes.clear();

    for (BoolItem* b : boolMap)
        delete b;
    boolMap.clear();

    for (FloatItem* f : floatMap)
        delete f;
    floatMap.clear();
}
