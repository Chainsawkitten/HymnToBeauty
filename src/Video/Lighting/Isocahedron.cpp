#include "Isocahedron.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include <glm/glm.hpp>

namespace Video {

Isocahedron::Isocahedron(LowLevelRenderer* lowLevelRenderer) {
    // Isocahedron.
	const float X = 0.525731112119133606f;
	const float Z = 0.850650808352039932f;
	const float N = 0.0f;

    glm::vec3 vertices[12] = {
        glm::vec3(-X,  N, Z ),
	    glm::vec3( X,  N, Z ),
	    glm::vec3(-X,  N, -Z),
	    glm::vec3( X,  N, -Z),
	    glm::vec3( N,  Z,  X),
	    glm::vec3( N,  Z, -X),
	    glm::vec3( N, -Z,  X),
	    glm::vec3( N, -Z, -X),
	    glm::vec3( Z,  X,  N),
	    glm::vec3(-Z,  X,  N),
	    glm::vec3( Z, -X,  N),
	    glm::vec3(-Z, -X,  N)
    };

    // Calculate the error when compared to a perfect sphere.
	error = glm::length((vertices[0] + vertices[4] + vertices[1]) / 3.0f);

    uint16_t indices[60] = {
        0, 1, 4,
	    0, 4, 9,
	    9, 4, 5,
	    4, 8, 5,
	    4, 1, 8,
	    8, 1, 10,
	    8, 10, 3,
	    5, 8, 3,
	    5, 3, 2,
	    2, 3, 7,
	    7, 3, 10,
	    7, 10, 6,
	    7, 6, 11,
	    11, 6, 0,
	    0, 6, 1,
	    6, 10, 1,
	    9, 11, 0,
	    9, 2, 11,
	    9, 5, 2,
	    7, 11, 2
    };

    /// @todo Subdivide?

    // Create buffers.
    vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, sizeof(glm::vec3) * 12, vertices);
    indexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, sizeof(uint16_t) * 60, indices);

    // Create vertex attribute.
    VertexDescription::Attribute attribute = {};
    attribute.size = 3;
    attribute.type = VertexDescription::AttributeType::FLOAT;
    attribute.normalized = false;

    vertexDescription = lowLevelRenderer->CreateVertexDescription(1, &attribute, true);

    // Create geometry binding.
    geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer, GeometryBinding::IndexType::SHORT, indexBuffer);
}

Isocahedron::~Isocahedron() {
    delete geometryBinding;
    delete vertexBuffer;
    delete indexBuffer;
    delete vertexDescription;
}

const VertexDescription* Isocahedron::GetVertexDescription() const {
    return vertexDescription;
}

GeometryBinding* Isocahedron::GetGeometryBinding() {
    return geometryBinding;
}

uint16_t Isocahedron::GetIndexCount() const {
    return 60;
}

float Isocahedron::GetError() const {
    return error;
}

}
