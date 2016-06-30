#pragma once

#include "Geometry3D.hpp"


namespace Geometry {
    /// A cube.
    class Cube : public Geometry3D {
        public:
            /// Create new cube.
            Cube();
            
            /// Destructor
            ~Cube();
            
            /// Get all the vertices.
            /**
            * @return Array of vertices
            */
            Vertex* GetVertices() const final;
            
            /// Get the number of vertices.
            /**
            * @return The number of vertices
            */
            unsigned int GetVertexCount() const final;
            
            /// Get all the vertex indices.
            /**
            * @return Array of vertex indices
            */
            unsigned int* GetIndices() const final;
            
            /// Get the number of indices.
            /**
            * @return The number of vertex indices.
            */
            unsigned int GetIndexCount() const final;
            
        private:
            Vertex* vertexData = nullptr;
            unsigned int vertexCount = 0;
            
            unsigned int* indexData = nullptr;
            unsigned int indexCount = 0;
    };
}
