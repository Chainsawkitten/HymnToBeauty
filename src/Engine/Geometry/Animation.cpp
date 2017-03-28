#include "Animation.hpp"

#include <assimp/scene.h>
#include "MathFunctions.hpp"

using namespace Geometry;

Animation::Animation() {
    
}

Animation::Animation(const aiAnimation* aAnimation) {
    Load(aAnimation);
}

Animation::~Animation() {
    
}

void Animation::Load(const aiAnimation* aAnimation) {
    name = aAnimation->mName.data;
    duration = aAnimation->mDuration;
    ticksPerSecond = aAnimation->mTicksPerSecond;
    channels.resize(aAnimation->mNumChannels);
    
    // Load animation channels.
    for (std::size_t c = 0; c < channels.size(); ++c) {
        Animation::AnimChannel* channel = &channels[c];
        const aiNodeAnim* aChannel = aAnimation->mChannels[c];
        channel->trgNodeName = aChannel->mNodeName.data;
        channelIndexMap[channel->trgNodeName] = c;
        
        // Position
        channel->posKeys.resize(aChannel->mNumPositionKeys);
        for (std::size_t i = 0; i < channel->posKeys.size(); ++i) {
            Animation::AnimChannel::VectorKey* posKey = &channel->posKeys[i];
            aiVectorKey* aPosKey = &aChannel->mPositionKeys[i];
            posKey->time = aPosKey->mTime;
            CpyVec(posKey->value, aPosKey->mValue);
        }
        
        // Rotation
        channel->rotKeys.resize(aChannel->mNumRotationKeys);
        for (std::size_t  i = 0; i < channel->rotKeys.size(); ++i) {
            Animation::AnimChannel::QuatKey* rotKey = &channel->rotKeys[i];
            aiQuatKey* aRotKey = &aChannel->mRotationKeys[i];
            rotKey->time = aRotKey->mTime;
            //CpyQuat(rotKey->value, aRotKey->mValue);
            rotKey->value = aRotKey->mValue;
        }
        
        // Scale
        channel->sclKeys.resize(aChannel->mNumScalingKeys);
        for (std::size_t i = 0; i < channel->sclKeys.size(); ++i) {
            Animation::AnimChannel::VectorKey* sclKey = &channel->sclKeys[i];
            aiVectorKey* aSclKey = &aChannel->mScalingKeys[i];
            sclKey->time = aSclKey->mTime;
            CpyVec(sclKey->value, aSclKey->mValue);
        }
    }
}

const Animation::AnimChannel* Animation::FindChannel(const std::string& name) const {
    const auto& it = channelIndexMap.find(name);
    if (it != channelIndexMap.end())
        return &channels[it->second];
    return nullptr;
}

void Animation::CalcInterpolatedRotation(aiQuaternion& rotation, float animationTime, const Animation::AnimChannel* channel) {
    // We need two values to interpolate.
    if (channel->rotKeys.size() == 1) {
        rotation = channel->rotKeys[0].value;
        return;
    }
    
    std::size_t cKey = channel->FindRotKey(animationTime);
    std::size_t nKey = (cKey + 1); // TODO % size()?
    assert(nKey < channel->rotKeys.size());
    float deltaTime = (float)(channel->rotKeys[nKey].time - channel->rotKeys[cKey].time);
    float factor = (animationTime - (float)channel->rotKeys[cKey].time) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiQuaternion& startRotationQ = channel->rotKeys[cKey].value;
    const aiQuaternion& endRotationQ = channel->rotKeys[nKey].value;
    aiQuaternion::Interpolate(rotation, startRotationQ, endRotationQ, factor);
    rotation = rotation.Normalize();
}

void Animation::CalcInterpolatedPosition(glm::vec3& translation, float animationTime, const Animation::AnimChannel* channel) {
    // We need two values to interpolate.
    if (channel->posKeys.size() == 1) {
        translation = channel->posKeys[0].value;
        return;
    }
    
    std::size_t cKey = channel->FindPosKey(animationTime);
    std::size_t nKey = (cKey + 1); // TODO % size()?
    assert(nKey < channel->posKeys.size());
    float deltaTime = (float)(channel->posKeys[nKey].time - channel->posKeys[cKey].time);
    float factor = (animationTime - (float)channel->posKeys[cKey].time) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const glm::vec3& start = channel->posKeys[cKey].value;
    const glm::vec3& end = channel->posKeys[nKey].value;
    const glm::vec3 delta = end - start;
    translation = start + factor * delta;
}

void Animation::CalcInterpolatedScaling(glm::vec3& scaling, float animationTime, const Animation::AnimChannel* channel) {
    // We need two values to interpolate.
    if (channel->sclKeys.size() == 1) {
        scaling = channel->sclKeys[0].value;
        return;
    }
    
    std::size_t cKey = channel->FindSclKey(animationTime);
    std::size_t nKey = (cKey + 1); // TODO % size()?
    assert(nKey < channel->sclKeys.size());
    float deltaTime = (float)(channel->sclKeys[nKey].time - channel->sclKeys[cKey].time);
    float factor = (animationTime - (float)channel->sclKeys[cKey].time) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const glm::vec3& start = channel->sclKeys[cKey].value;
    const glm::vec3& end = channel->sclKeys[nKey].value;
    const glm::vec3 delta = end - start;
    scaling = start + factor * delta;
}

std::size_t Animation::AnimChannel::FindRotKey(float animationTime) const {
    assert(rotKeys.size() > 0);
    for (std::size_t i = 0; i < rotKeys.size() - 1; ++i)
        if (animationTime < (float)rotKeys[i + 1].time)
            return i;
    assert(0);
    return -1;
}
std::size_t Animation::AnimChannel::FindPosKey(float animationTime) const {
    assert(posKeys.size() > 0);
    for (std::size_t i = 0; i < posKeys.size() - 1; ++i)
        if (animationTime < (float)posKeys[i + 1].time)
            return i;
    assert(0);
    return -1;
}

std::size_t Animation::AnimChannel::FindSclKey(float animationTime) const {
    assert(sclKeys.size() > 0);
    for (std::size_t i = 0; i < sclKeys.size() - 1; ++i)
        if (animationTime < (float)sclKeys[i + 1].time)
            return i;
    assert(0);
    return -1;
}
