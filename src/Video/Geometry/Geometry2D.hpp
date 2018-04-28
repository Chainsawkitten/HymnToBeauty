#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Video {
    namespace Geometry {
        /// Interface for renderable 2D geometry.
        class Geometry2D {
            public:
                /// A vertex point.
                struct Vertex {
                    /// Position.
                    glm::vec2 position;

                    /// %Texture coordinate.
                    glm::vec2 textureCoordinate;
                };

                /// Destructor
                virtual ~Geometry2D();

                /// Get all the vertices.
                /**
                 * @return Array of vertices
                 */
                virtual Vertex* GetVertices() const = 0;

                /// Get the number of vertices.
                /**
                 * @return The number of vertices
                 */
                virtual unsigned int GetVertexCount() const = 0;

                /// Get all the vertex indices.
                /**
                 * @return Array of vertex indices
                 */
                virtual unsigned int* GetIndices() const = 0;

                /// Get the number of indicies.
                /**
                 * @return The number of vertex indices.
                 */
                virtual unsigned int GetIndexCount() const = 0;

                /// Get the vertex array.
                /**
                 * @return The vertex array
                 */
                GLuint GetVertexArray() const;

            protected:
                /// Generate vertex and index buffers.
                void GenerateBuffers();

                /// Generate vertex array.
                void GenerateVertexArray();

            private:
                GLuint vertexBuffer;
                GLuint indexBuffer;
                GLuint vertexArray;
        };
    }
}
