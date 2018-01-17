#pragma once

#include <Video/Geometry/Geometry2D.hpp>

namespace Video {
    namespace Geometry {
        /// A renderable 2D rectangle.
        class Rectangle : public Video::Geometry::Geometry2D {
            public:
                /// Constructor.
                Rectangle();
                
                /// Destructor.
                ~Rectangle() final;
                
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
                
                /// Get the number of indicies.
                /**
                 * @return The number of vertex indices.
                 */
                unsigned int GetIndexCount() const final;
                
            private:
                Vertex* vertexData = nullptr;
                unsigned int vertexNr = 0;
                
                unsigned int* indexData = nullptr;
                unsigned int indexNr = 0;
        };
    }
}
