#pragma once

#include "Geometry3D.hpp"
#include <vector>
#include <fstream>

namespace Geometry {
    /// A model loaded from an OBJ file.
    class OBJModel : public Geometry3D {
    public:
        /// Create new empty OBJ model.
        /**
         * The created model has to be loaded later using Load.
         */
        OBJModel();
        
        /// Create new model from OBJ file.
        /**
         * @param filename Filename (relative or absolute) to the OBJ model file.
         */
        OBJModel(const char* filename);

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
        
        /// Load model from OBJ file.
        /**
         * @param filename Filename (relative or absolute) to the OBJ model file.
         */
        void Load(const char* filename);
    
    private:
        struct Face {
            struct Vertex {
                unsigned int positionIndex;
                unsigned int normalIndex;
                unsigned int textureIndex;
            };

            Vertex vertices[3];
        };
        
        static glm::vec3 ReadVec3(std::ifstream& modelFile);
        static glm::vec2 ReadVec2(std::ifstream& modelFile);
        static Face ReadFace(std::ifstream& modelFile);
        static Face::Vertex ReadVertex(std::ifstream& modelFile);
        static void CalculateTangents(std::vector<Face>& faces, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& textureCoordinates, std::vector<glm::vec3>& tangents);

        Vertex* vertexData = nullptr;
        unsigned int vertexNr = 0;

        unsigned int* indexData = nullptr;
        unsigned int indexNr = 0;
    };
}
