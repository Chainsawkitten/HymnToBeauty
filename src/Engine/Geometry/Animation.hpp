#pragma once

#include <assimp/quaternion.h>
#include <glm/glm.hpp>
//#include <map>
#include <string>
#include <vector>

struct aiAnimation;

namespace Geometry {
    /// An animation loaded from a file.
    class Animation {
        public:
            /// Animation channel representing rotation, position and scale transformations.
            struct AnimChannel {
                struct QuatKey {
                    double time;
                    aiQuaternion value;
                };
                struct VectorKey {
                    double time;
                    glm::vec3 value;
                };

                std::size_t FindRotKey(float animationTime) const;
                std::size_t FindPosKey(float animationTime) const;
                std::size_t FindSclKey(float animationTime) const;

                std::vector<QuatKey> rotKeys;
                std::vector<VectorKey> posKeys;
                std::vector<VectorKey> sclKeys;
                std::string trgNodeName = "";
            };

            /// Create new empty animation.
            /**
             * The created animation has to be loaded later using Load.
             */
            Animation();

            /// Create new animation.
            /**
             * @param aScene Pointer to assimp animation.
             */
            Animation(const aiAnimation* aAnimation);

            /// Destructor.
            ~Animation();

            /// Load animation.
            /**
             * @param aScene Pointer to assimp animation.
             */
            void Load(const aiAnimation* aAnimation);

            //TODO COMMENTS
            const AnimChannel* FindChannel(const std::string& nodeName) const;

            static void CalcInterpolatedScaling(glm::vec3& scaling, float animationTime, const AnimChannel* channel);
            static void CalcInterpolatedRotation(aiQuaternion& rotation, float animationTime, const Animation::AnimChannel* channel);
            static void CalcInterpolatedPosition(glm::vec3& translation, float animationTime, const Animation::AnimChannel* channel);

            std::string name;
            double duration;
            double ticksPerSecond;
        private:
            //std::map<std::string, std::size_t> channelIndexMap; 
            std::vector<AnimChannel> channels;
        };
}
