#pragma once

#include "Geometry3D.hpp"
#include <json/json.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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

        static Assimp::Importer aiImporter;

        struct Mesh {
            aiMesh* assimpMesh = nullptr;
            aiNode* rootNode = nullptr;
            unsigned int vertexNr = 0, indexNr = 0;
            Vertex* vertexData = nullptr;
            unsigned int* indexData = nullptr;
            void Clear() {
                if (vertexData != nullptr) {
                    delete[] vertexData;
                    vertexData = nullptr;
                    vertexNr = 0;
                }
                if (indexData != nullptr) {
                    indexData = nullptr;
                    indexNr = 0;
                }
            }
        };

        static void LoadMesh(const aiMesh* assimpMesh, Mesh& mesh);

        static void TransfromMesh(Mesh& mesh);

        // https://github.com/ccxvii/asstools/blob/master/assview.c
        // Find a node by name in the hierarchy (for anims and bones).
        static aiNode* FindNode(aiNode* node, const char* name);
    
        // Calculate absolute transform for node to do mesh skinning.
        static void TransformNode(const aiNode* node, aiMatrix4x4& transformMat);

        // Animate mesh
        static void AninmateMesh(const aiAnimation* animation, const float tick, Mesh& mesh);

        static void Mix(aiQuaternion& q1, const aiQuaternion& q2, float t, aiQuaternion& result);
        static float Dot(const aiQuaternion& q1, const aiQuaternion& q2);
        static void Normalize(aiQuaternion& q);
        static void ComposeMatrix(const aiQuaternion& r, aiMatrix4x4& matrix);

        static void CpyVec(const aiVector3D& aiVec, glm::vec3& glmVec);
        static void CpyVec(const aiVector3D& aiVec, glm::vec2& glmVec);
        static void CpyVec(const aiVector2D& aiVec, glm::vec2& glmVec);
        //static void CpyMat(const aiMatrix4x4& aiMat, glm::mat4& glmMat);
        static void CpyMat(const aiMatrix4x4& aiMat4, aiMatrix3x3& aiMat3);

        Vertex* vertexData = nullptr;
        unsigned int vertexNr = 0;

        unsigned int* indexData = nullptr;
        unsigned int indexNr = 0;
    };
}
