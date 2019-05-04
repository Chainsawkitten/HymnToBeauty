#include "Rectangle.hpp"

using namespace Video;
using namespace Geometry;

Rectangle::Rectangle(LowLevelRenderer* lowLevelRenderer) : Geometry2D(lowLevelRenderer) {
    // Vertices.
    vertexNr = 4;
    vertexData = new Vertex[vertexNr];

    vertexData[0] = {glm::vec2(-1.f, 1.f), glm::vec2(0.f, 1.f)};
    vertexData[1] = {glm::vec2(1.f, 1.f), glm::vec2(1.f, 1.f)};
    vertexData[2] = {glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f)};
    vertexData[3] = {glm::vec2(1.f, -1.f), glm::vec2(1.f, 0.f)};

    // Vertex indices.
    indexNr = 6;
    indexData = new unsigned int[indexNr];

    indexData[0] = 0;
    indexData[1] = 2;
    indexData[2] = 1;
    indexData[3] = 1;
    indexData[4] = 2;
    indexData[5] = 3;

    GenerateBuffers();
    GenerateGeometryBinding();
}

Rectangle::~Rectangle() {
    delete[] vertexData;
    delete[] indexData;
}

const Geometry2D::Vertex* Rectangle::GetVertices() const {
    return vertexData;
}

unsigned int Rectangle::GetVertexCount() const {
    return vertexNr;
}

const unsigned int* Rectangle::GetIndices() const {
    return indexData;
}

unsigned int Rectangle::GetIndexCount() const {
    return indexNr;
}
