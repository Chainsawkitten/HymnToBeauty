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

        struct Face {
            struct Vertex {
                unsigned int positionIndex;
                unsigned int normalIndex;
                unsigned int textureIndex;
            };

            Vertex vertices[3];
        };

        static void CpyVec(const aiVector3D& aiVec, glm::vec3& glmVec);
        static void CpyVec(const aiVector3D& aiVec, glm::vec2& glmVec);
        static void CpyVec(const aiVector2D& aiVec, glm::vec2& glmVec);
        static void CpyMat(const aiMatrix4x4& aiMat, glm::mat4& glmMat);

        Vertex* vertexData = nullptr;
        unsigned int vertexNr = 0;

        unsigned int* indexData = nullptr;
        unsigned int indexNr = 0;
    };
}
