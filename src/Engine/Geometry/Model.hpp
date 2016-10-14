#pragma once

#include "Geometry3D.hpp"
#include <json/json.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
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

    private:
        /// The extension of the model.
        std::string extension;

        static Assimp::Importer aiImporter;

        struct Mesh {
            unsigned int vertexNr = 0, indexNr = 0;
            Vertex* vertexData = nullptr;
            unsigned int* indexData = nullptr;
            void Clear();
        } mesh;

        struct Skeleton {
            struct Node {
                std::string name;
                glm::mat4 transformation;
				glm::mat4 inverseTransform;
                Node* parent;
                unsigned int childrenNr = 0;
                Node** childrenData = nullptr;
            };
            struct Bone {
                std::string name;
                glm::mat4 offsetMatrix;
				glm::mat4 inverseOffsetMatrix;
                struct VertexWeight {
                    unsigned int vID;
                    float weight;
                };
                unsigned int weightNr = 0;
                VertexWeight* weightData = nullptr;
                void Clear();
            } bone;
            Node rootNode;
            std::map<std::string, Bone> boneMap; // TODOD private

            Node* FindNode(const char* name);
            Bone* FindBone(const char* name);
            void Load(aiNode* assimpRootNode, aiMesh* assimpMesh);
            void Clear();
        private:
           // std::map<std::string, Bone> boneMap;

            void BuildSkeleton(aiNode* assimpNode, Node* node, Node* parentNode);
            Node* FindNode(Node* node, const char* name);
            void Clear(Node* node);
        } skeleton;

        struct Animation {
            double duration;
            double ticksPerSecond;
            struct AnimChanel {
                std::string trgNodeName = "";
                struct QuatKey {
                    double time;
                    glm::quat value;
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
            };
            unsigned int animChanelNr = 0;
            AnimChanel* animChanelData = nullptr;

            void Clear();
        } animation;

        static void LoadMesh(aiMesh* assimpMesh, Mesh& mesh);
        static void LoadSkeleton(aiNode* rootNode, aiMesh* assimpMesh, Skeleton& skeleton);
        static void LoadAnimation(aiAnimation* assimpAnimation, Animation& animation);

        // TMP
        aiMesh* tmpAssimpMesh;
		aiAnimation* tmpAssimpAnimation;
        void TransfromMesh(Skeleton& skeleton);
        void TransformNode(const Skeleton::Node* currentNode, glm::mat4& transfromMatrix);
        void AnimateSkeleton(const Animation& animation, const float tick);
        

        // https://github.com/ccxvii/asstools/blob/master/assview.c
        //static void TransfromMesh(aiNode* rootNode, Mesh& mesh);
        //static void TransformNode(const aiNode* node, aiMatrix4x4& transformMat);
        //static void AninmateMesh(const aiAnimation* animation, const float tick, Mesh& mesh);
		aiNode* Model::FindNode(aiNode* node, const char* name);

        //static void Mix(aiQuaternion& q1, const aiQuaternion& q2, float t, aiQuaternion& result);
        //static float Dot(const aiQuaternion& q1, const aiQuaternion& q2);
        //static void Normalize(aiQuaternion& q);
        //static void ComposeMatrix(const aiQuaternion& r, aiMatrix4x4& matrix);

        static void MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result);
        static void MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result);
        static float DotQuat(const glm::quat& q1, const glm::quat& q2);
        static void NormalizeQuat(glm::quat& q);
        static void ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& matrix);

        static void CpyVec(const aiVector3D& aiVec, glm::vec3& glmVec);
        static void CpyVec(const aiVector3D& aiVec, glm::vec2& glmVec);
        static void CpyVec(const aiVector2D& aiVec, glm::vec2& glmVec);
        static void CpyMat(const aiMatrix4x4& aiMat, glm::mat4& glmMat);
        static void CpyMat(const aiMatrix4x4& aiMat4, aiMatrix3x3& aiMat3);
        static void CpyQuat(const aiQuaternion& aiQuat, glm::quat &glmQuat);

        Vertex* vertexData = nullptr;
        unsigned int vertexNr = 0;

        unsigned int* indexData = nullptr;
        unsigned int indexNr = 0;
    };
}
