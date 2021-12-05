#include "StaticVertex.hpp"

#include "../../LowLevelRenderer/Interface/LowLevelRenderer.hpp"

using namespace Video;
using namespace Geometry;

Buffer* VertexType::StaticVertex::GenerateVertexBuffer(LowLevelRenderer* lowLevelRenderer, StaticVertex* vertices, unsigned int vertexCount) {
    return lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, vertexCount * sizeof(StaticVertex), vertices);
}

VertexDescription* VertexType::StaticVertex::GenerateVertexDescription(LowLevelRenderer* lowLevelRenderer) {
    VertexDescription::Attribute attributes[4];
    attributes[0].size = 3;
    attributes[0].type = VertexDescription::AttributeType::FLOAT;
    attributes[0].normalized = false;

    attributes[1].size = 2;
    attributes[1].type = VertexDescription::AttributeType::FLOAT;
    attributes[1].normalized = false;

    attributes[2].size = 3;
    attributes[2].type = VertexDescription::AttributeType::FLOAT;
    attributes[2].normalized = false;

    attributes[3].size = 3;
    attributes[3].type = VertexDescription::AttributeType::FLOAT;
    attributes[3].normalized = false;

    return lowLevelRenderer->CreateVertexDescription(4, attributes, true);
}
