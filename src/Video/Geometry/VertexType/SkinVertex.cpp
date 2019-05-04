#include "SkinVertex.hpp"

#include "../../LowLevelRenderer/Interface/LowLevelRenderer.hpp"

using namespace Video;
using namespace Geometry;

Buffer* VertexType::SkinVertex::GenerateVertexBuffer(LowLevelRenderer* lowLevelRenderer, SkinVertex* vertices, unsigned int vertexCount) {
    return lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, vertexCount * sizeof(SkinVertex), vertices);
}

VertexDescription* VertexType::SkinVertex::GenerateVertexDescription(LowLevelRenderer* lowLevelRenderer) {
    VertexDescription::Attribute attributes[6];
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

    attributes[4].size = 4;
    attributes[4].type = VertexDescription::AttributeType::INT;
    attributes[4].normalized = false;

    attributes[5].size = 4;
    attributes[5].type = VertexDescription::AttributeType::FLOAT;
    attributes[5].normalized = false;

    return lowLevelRenderer->CreateVertexDescription(6, attributes, true);
}
