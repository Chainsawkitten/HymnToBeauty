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

                Vertex* GetVertices() const final;
                unsigned int GetVertexCount() const final;
                unsigned int* GetIndices() const final;
                unsigned int GetIndexCount() const final;

            private:
                Vertex* vertexData = nullptr;
                unsigned int vertexNr = 0;

                unsigned int* indexData = nullptr;
                unsigned int indexNr = 0;
        };
    }
}
