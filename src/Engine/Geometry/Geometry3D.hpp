#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Physics/AxisAlignedBoundingBox.hpp"

namespace Geometry {
    /// Interface for renderable 3D geometry.
    /**
     * Can be rendered using the default shaders.
     */
    class Geometry3D {
        public:
            /// A vertex point.
            struct Vertex {
                /// Position.
                glm::vec3 position;
                /// %Texture coordinate.
                glm::vec2 textureCoordinate;
                /// Normal.
                glm::vec3 normal;
                /// Tangent vector.
                glm::vec3 tangent;
            };
            
            /// Destructor
            virtual ~Geometry3D();
            
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
            
            /// Get the axis-aligned bounding box around the geometry.
            /**
             * @return Local space axis-aligned bounding box around the geometry.
             */
            const Physics::AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() const;
            
        protected:
            /// Generate vertex and index buffers.
            void GenerateBuffers();
            
            /// Generate vertex array.
            void GenerateVertexArray();
            
            /// Create local space axis-aligned bounding box around the geometry.
            void CreateAxisAlignedBoundingBox();
            
        private:
            GLuint vertexBuffer;
            GLuint indexBuffer;
            GLuint vertexArray;
            
            Physics::AxisAlignedBoundingBox axisAlignedBoundingBox;

    };
}

