#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Geometry2D.hpp"
#include "../Shader/ShaderProgram.hpp"

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
            
            /// Render the rectangle with a solid color.
            /**
             * @param position Position on the screen in pixels.
             * @param size Size in pixels.
             * @param color Color.
             * @param screenWidth Width of the screen in pixels.
             * @param screenHeight Height of the screen in pixels.
             */
            void Render(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, int screenWidth, int screenHeight) const;
            
        private:
            Vertex *vertexData = nullptr;
            unsigned int vertexNr = 0;
            
            unsigned int* indexData = nullptr;
            unsigned int indexNr = 0;
            
            // Shaders
            Shader* vertexShader;
            Shader* fragmentShader;
            ShaderProgram* shaderProgram;
            
    };
}

/** @} */

#endif
