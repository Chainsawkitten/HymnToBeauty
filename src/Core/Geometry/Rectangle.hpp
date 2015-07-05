#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Geometry2D.hpp"

/** @ingroup Core
 * @{
 */

namespace Geometry {
    /// A renderable 2D rectangle.
    class Rectangle : public Geometry2D {
        public:
            /// Constructor.
            Rectangle();
            
            /// Destructor.
            ~Rectangle();
            
            /// Get all the vertices.
            /**
             * @return Array of vertices
             */
            Vertex* Vertices() const;
            
            /// Get the number of vertices.
            /**
             * @return The number of vertices
             */
            unsigned int VertexCount() const;
            
            /// Get all the vertex indices.
            /**
             * @return Array of vertex indices
             */
            unsigned int* Indices() const;
            
            /// Get the number of indicies.
            /**
             * @return The number of vertex indices.
             */
            unsigned int IndexCount() const;
            
        private:
            Vertex *vertexData = nullptr;
            unsigned int vertexNr = 0;
            
            unsigned int* indexData = nullptr;
            unsigned int indexNr = 0;
            
    };
}

/** @} */

#endif
