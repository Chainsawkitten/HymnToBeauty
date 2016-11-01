#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <json/json.h>
#include "../Physics/AxisAlignedBoundingBox.hpp"

namespace Geometry {
    /// Renderable 3D geometry.
    class Geometry3D {
    public:
        /// Type of layout of vertex points.
        enum Type {
            STATIC = 0,
            SKIN
        };

        /// Destructor.
        virtual ~Geometry3D();

        /// Get the vertex array.
        /**
         * @return The vertex array.
         */
        const GLuint GetVertexArray() const;

        /// Get number of indices.
        /**
         * @return Index count.
         */
        const unsigned int GetIndexCount() const;

        /// Get the axis-aligned bounding box around the geometry.
        /**
         * @return Local space axis-aligned bounding box around the geometry.
         */
        const Physics::AxisAlignedBoundingBox& GetAxisAlignedBoundingBox() const;

        /// Get geometry type.
        /**
         * @return Type.
         */
        virtual const Type GetType() const = 0;

    protected:
        /// Generate vertex buffer.
        virtual void GenerateVertexBuffer(GLuint& vertexBuffer) = 0;

        /// Generate vertex array.
        virtual void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) = 0;

        /// Generate index buffer.
        void GenerateIndexBuffer(unsigned int* indexData, unsigned int indexCount, GLuint& indexBuffer);

        /// Create local space axis-aligned bounding box around the geometry.
        void CreateAxisAlignedBoundingBox(const std::vector<glm::vec3*>& positions);

        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint vertexArray;

    private:
        Physics::AxisAlignedBoundingBox axisAlignedBoundingBox;
        unsigned int indexCount = 0;
    };
}
