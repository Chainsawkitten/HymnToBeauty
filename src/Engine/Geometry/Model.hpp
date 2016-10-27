#pragma once

#include "Geometry3D.hpp"
#include <json/json.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Geometry {
    /// A model loaded from an file.
    class Model : public Geometry3D {
    public:
        /// Create new empty model.
        /**
         * The created model has to be loaded later using Load.
         */
        Model();
        
        /// Create new model from file.
        /**
         * @param filename Filename (relative or absolute) to the model file.
         */
        Model(const char* filename);

        /// Remove model
        ~Model();

        /// Get all the vertices.
        /**
         * @return Array of vertices
         */
        Vertex* GetVertices() const;

        /// Get the number of vertices.
        /**
         * @return The number of vertices
         */
        unsigned int GetVertexCount() const;

        /// Get all the vertex indices.
        /**
         * @return Array of vertex indices
         */
        unsigned int* GetIndices() const;

        /// Get the number of indicies.
        /**
         * @return The number of vertex indices.
         */
        unsigned int GetIndexCount() const;
        
        /// Save the model.
        /**
         * @return JSON value to be stored on disk.
         */
        Json::Value Save() const;
        
        /// Load model from JSON node.
        /**
         * @param node JSON node to load from.
         */
        void Load(const Json::Value& node);
        
        /// Load model from file.
        /**
         * @param filename Filename (relative or absolute) to the model file.
         */
        void Load(const char* filename);
        
        /// The name of the model.
        std::string name;

        /// The extension of the model.
        std::string extension;

    private:
        #define NUM_BONES_PER_VERTEX 4
        #define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
        static Assimp::Importer aiImporter;
        const aiScene* aScene;

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
        Node rootNode;
        std::vector<MeshEntry> entries;
        std::map<std::string, unsigned int> boneMap;
        unsigned int numBones = 0;
        std::vector<BoneInfo> boneInfos;
        std::vector<VertexBoneData> boneVertDatas;
        glm::mat4 globalInverseTransform;


        //struct Mesh {
        //    unsigned int vertexNr = 0, indexNr = 0;
        //    Vertex* vertexData = nullptr;
        //    unsigned int* indexData = nullptr;
        //    //void Clear();
        //} mesh;

    //    struct Skeleton {
    //        struct Node {
    //            std::string name;
    //            glm::mat4 transformation;
				//glm::mat4 inverseTransform;
    //            Node* parent;
    //            unsigned int childrenNr = 0;
    //            Node** childrenData = nullptr;
    //        };
    //        struct Bone {
    //            std::string name;
    //            glm::mat4 offsetMatrix;
				//glm::mat4 inverseOffsetMatrix;
    //            struct VertexWeight {
    //                unsigned int vID;
    //                float weight;
    //            };
    //            unsigned int weightNr = 0;
    //            VertexWeight* weightData = nullptr;
    //            //void Clear();
    //        } bone;
    //        Node rootNode;
    //        std::map<std::string, Bone> boneMap;

    //        //Node* FindNode(const char* name);
    //        //Bone* FindBone(const char* name);
    //        //void Load(aiNode* assimpRootNode, aiMesh* assimpMesh);
    //        //void Clear();
    //    private:
    //       // std::map<std::string, Bone> boneMap;

    //        //void BuildSkeleton(aiNode* assimpNode, Node* node, Node* parentNode);
    //        //Node* FindNode(Node* node, const char* name);
    //        //void Clear(Node* node);
    //    } skeleton;

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
        std::vector<Animation> animations;

        void LoadMeshes(const aiScene* aScene);
        void LoadAnimations(const aiScene* aScene);
        void LoadNodeTree(aiNode* aNode, Node* node, Node* parentNode);
        void DebugDrawSkeleton();
        Node* FindNodeInNodeTree(Node* node, const std::string& name);
        void NodeTransform(const Node* node, glm::mat4& transform);

        void BoneTransform(float timeInSeconds, const Animation& animation);
        void ReadNodeHeirarchy(float animationTime, Node* node, const glm::mat4& parentTransform);
        void CalcInterpolatedScaling(glm::vec3& scaling, float animationTime, const Animation::AnimChanel* animChanel);
        void CalcInterpolatedRotation(aiQuaternion& rotation, float animationTime, const Animation::AnimChanel* animChanel);
        void CalcInterpolatedPosition(glm::vec3& translation, float animationTime, const Animation::AnimChanel* animChanel);
        void MeshTransform(const std::vector<glm::mat4>& transforms);
        //static void LoadMesh(aiMesh* assimpMesh, Mesh& mesh);
        //static void LoadSkeleton(aiNode* rootNode, aiMesh* assimpMesh, Skeleton& skeleton);
        //static void LoadAnimation(aiAnimation* assimpAnimation, Animation& animation);

        // TMP
        //const aiScene* tmpScene;
        //aiMesh* tmpAssimpMesh;
		//aiAnimation* tmpAssimpAnimation;
        //void TransfromMesh(Skeleton& skeleton);
        //void TransformNode(const Skeleton::Node* currentNode, glm::mat4& transfromMatrix);
        //void AnimateSkeleton(const Animation& animation, const float tick);
        

        // https://github.com/ccxvii/asstools/blob/master/assview.c
        //static void TransfromMesh(aiNode* rootNode, Mesh& mesh);
        //static void TransformNode(const aiNode* node, aiMatrix4x4& transformMat);
        //static void AninmateMesh(const aiAnimation* animation, const float tick, Mesh& mesh);
		//aiNode* Model::FindNode(aiNode* node, const char* name);

        //static void Mix(aiQuaternion& q1, const aiQuaternion& q2, float t, aiQuaternion& result);
        //static float Dot(const aiQuaternion& q1, const aiQuaternion& q2);
        //static void Normalize(aiQuaternion& q);
        //static void ComposeMatrix(const aiQuaternion& r, aiMatrix4x4& matrix);

        static void MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result);
        static void MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result);
        static float DotQuat(const glm::quat& q1, const glm::quat& q2);
        static void NormalizeQuat(glm::quat& q);
        static void ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& matrix);

        static void QuatToMat(glm::mat4& m, const glm::quat& q);
        static void CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec);
        static void CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec);
        static void CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec);
        static void CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat);
        static void CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4);
        static void CpyQuat(glm::quat &glmQuat, const aiQuaternion& aiQuat);

        Vertex* vertexData = nullptr;
        unsigned int vertexNr = 0;

        unsigned int* indexData = nullptr;
        unsigned int indexNr = 0;
    };
}
