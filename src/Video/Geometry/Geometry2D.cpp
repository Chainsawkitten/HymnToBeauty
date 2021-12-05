#include "Geometry2D.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../LowLevelRenderer/Interface/Buffer.hpp"
#include "../LowLevelRenderer/Interface/VertexDescription.hpp"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

using namespace Video;
using namespace Geometry;

Geometry2D::Geometry2D(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;
}

Geometry2D::~Geometry2D() {
    delete geometryBinding;
    delete vertexDescription;
    delete vertexBuffer;
    delete indexBuffer;
}

const GeometryBinding* Geometry2D::GetGeometryBinding() const {
    return geometryBinding;
}

void Geometry2D::GenerateBuffers() {
    vertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, GetVertexCount() * sizeof(Vertex), GetVertices());
    indexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, GetIndexCount() * sizeof(unsigned int), GetIndices());
}

void Geometry2D::GenerateGeometryBinding() {
    // Define vertex data layout.
    VertexDescription::Attribute attributes[2];
    attributes[0].size = 2;
    attributes[0].type = VertexDescription::AttributeType::FLOAT;
    attributes[0].normalized = false;

    attributes[1].size = 2;
    attributes[1].type = VertexDescription::AttributeType::FLOAT;
    attributes[1].normalized = false;

    // Create vertex description.
    vertexDescription = lowLevelRenderer->CreateVertexDescription(2, attributes, true);

    // Create geometry binding.
    geometryBinding = lowLevelRenderer->CreateGeometryBinding(vertexDescription, vertexBuffer, Video::GeometryBinding::IndexType::INT, indexBuffer);
}
