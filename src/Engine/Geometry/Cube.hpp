#pragma once

#include "Geometry3D.hpp"
#include "VertexType/StaticVertex.hpp"

namespace Geometry {
    /// A cube.
    class Cube : public Geometry3D {
        public:
            /// Create new cube.
            Cube();
            
            /// Destructor.
            ~Cube() final;

            /// Get geometry type.
            /**
             * @return Type.
             */
            Type GetType() const;

        protected:
            /// Generate vertex buffer.
            void GenerateVertexBuffer(GLuint& vertexBuffer);

            /// Generate vertex array.
            void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray);

        private:
            std::vector<VertexType::StaticVertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<glm::vec3*> verticesPos;
    };
}
