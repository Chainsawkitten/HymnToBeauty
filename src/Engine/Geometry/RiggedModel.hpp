#pragma once

#include "Model.hpp"
#include <vector>
#include "VertexType/SkinVertex.hpp"
#define NUM_BONES_PER_VERTEX 4
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

struct aiNode;

namespace Geometry {
    /// A model loaded from an file.
    class RiggedModel : public Model {
        public:
            /// Create new empty rigged model.
            /**
             * The created rigged model has to be loaded later using Load.
             */
            RiggedModel();

            /// Create new rigged model from file.
            /**
             * @param filename Filename (relative or absolute) to the model file.
             */
            RiggedModel(const char* filename);

            /// Destructor.
            ~RiggedModel() final;

            /// Load rigged model from file.
            /**
             * @param filename Filename (relative or absolute) to the model file.
             */
            void RiggedModel::Load(const char* filename);

            /// Get geometry type.
            /**
             * @return Type.
             */
            const Type GetType() const;

        protected:
            /// Generate vertex buffer.
            void GenerateVertexBuffer(GLuint& vertexBuffer);

            /// Generate vertex array.
            void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray);

        private:
            struct VertexBoneData {
                unsigned int boneIDs[NUM_BONES_PER_VERTEX];
                float weights[NUM_BONES_PER_VERTEX];
                void AddBoneData(unsigned int boneID, float weight) {
                    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(boneIDs); i++) {
                        if (weights[i] == 0.0) {
                            boneIDs[i] = boneID;
                            weights[i] = weight;
                            return;
                        }
                    }
                    // Should never get here - more bones than we have space for.
                    assert(0);
                }
            };

            struct BoneInfo {
                glm::mat4 boneOffset;
                glm::mat4 finalTransformation;
            };

            struct MeshEntry {
                unsigned int numIndices = 0;
                unsigned int baseVertex = 0;
                unsigned int baseIndex = 0;
            };

            struct Node {
                std::string name;
                glm::mat4 transformation;
                Node* parent;
                std::vector<Node> children;
            };

            struct Animation {
                std::string name;
                double duration;
                double ticksPerSecond;
                struct AnimChanel {
                    std::string trgNodeName = "";
                    struct QuatKey {
                        double time;
                        aiQuaternion value;
                    };
                    unsigned int rotKeyNr = 0;
                    QuatKey* rotKeyData = nullptr;
                    struct VectorKey {
                        double time;
                        glm::vec3 value;
                    };
                    unsigned int posKeyNr = 0;
                    VectorKey* posKeyData = nullptr;
                    unsigned int scaleKeyNr = 0;
                    VectorKey* scaleKeyData = nullptr;

                    unsigned int FindScaling(float animationTime) const {
                        assert(scaleKeyNr > 0);
                        for (unsigned int i = 0; i < scaleKeyNr - 1; ++i)
                            if (animationTime < (float)scaleKeyData[i + 1].time)
                                return i;
                        assert(0);
                        return 0;
                    }
                    unsigned int FindRotation(float animationTime) const {
                        assert(rotKeyNr > 0);
                        for (unsigned int i = 0; i < rotKeyNr - 1; ++i)
                            if (animationTime < (float)rotKeyData[i + 1].time)
                                return i;
                        assert(0);
                        return 0;
                    }
                    unsigned int FindPosition(float animationTime) const {
                        assert(posKeyNr > 0);
                        for (unsigned int i = 0; i < posKeyNr - 1; ++i)
                            if (animationTime < (float)posKeyData[i + 1].time)
                                return i;
                        assert(0);
                        return 0;
                    }
                };
                unsigned int animChanelNr = 0;
                AnimChanel* animChanelData = nullptr;

                const AnimChanel* FindAnimChanel(const std::string& nodeName) const {
                    for (unsigned int i = 0; i < this->animChanelNr; ++i) {
                        const AnimChanel* animChanel = &this->animChanelData[i];
                        if (animChanel->trgNodeName == nodeName) {
                            return animChanel;
                        }
                    }
                    return nullptr;
                }
                //void Clear();
            };

            void LoadMeshes(const aiScene* aScene);
            void LoadAnimations(const aiScene* aScene);
            void LoadNodeTree(aiNode* aNode, Node* node, Node* parentNode);
            void NodeTransform(const Node* node, glm::mat4& transform);

            // TMP
            void BoneTransform(float timeInSeconds, const Animation& animation);
            void ReadNodeHeirarchy(float animationTime, Node* node, const glm::mat4& parentTransform);
            void CalcInterpolatedScaling(glm::vec3& scaling, float animationTime, const Animation::AnimChanel* animChanel);
            void CalcInterpolatedRotation(aiQuaternion& rotation, float animationTime, const Animation::AnimChanel* animChanel);
            void CalcInterpolatedPosition(glm::vec3& translation, float animationTime, const Animation::AnimChanel* animChanel);
            void MeshTransform(const std::vector<glm::mat4>& transforms);
            // ~TMP

            std::vector<VertexType::SkinVertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<glm::vec3*> verticesPos;
            Node rootNode;
            std::map<std::string, unsigned int> boneMap;
            unsigned int numBones = 0;
            std::vector<BoneInfo> boneInfos;
            std::vector<VertexBoneData> boneVertDatas;
            glm::mat4 globalInverseTransform;
            std::vector<Animation> animations;
    };
}
