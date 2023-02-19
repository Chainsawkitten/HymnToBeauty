#include "WebGPUVertexDescription.hpp"

#include <Utility/Log.hpp>
#include <cassert>

namespace Video {

WebGPUVertexDescription::WebGPUVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes) {
    assert(attributeCount > 0);
    assert(attributes != nullptr);

    vertexBufferLayout = {};
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
    vertexBufferLayout.attributeCount = attributeCount;
    
    vertexAttributes = new WGPUVertexAttribute[attributeCount];
    vertexBufferLayout.attributes = vertexAttributes;

    for (unsigned int i = 0; i < attributeCount; ++i) {
        vertexAttributes[i].shaderLocation = i;
        vertexAttributes[i].offset = vertexBufferLayout.arrayStride;

        uint64_t size = 0;
        switch (attributes[i].type) {
        case VertexDescription::AttributeType::UNSIGNED_BYTE:
            size = sizeof(uint8_t) * attributes[i].size;
            switch (attributes[i].size) {
            case 1:
                assert(false);
                break;
            case 2:
                vertexAttributes[i].format = (attributes[i].normalized ? WGPUVertexFormat_Unorm8x2 : WGPUVertexFormat_Uint8x2);
                break;
            case 3:
                assert(false);
                break;
            case 4:
                vertexAttributes[i].format = (attributes[i].normalized ? WGPUVertexFormat_Unorm8x4 : WGPUVertexFormat_Uint8x4);
                break;
            default:
                assert(false);
            }
            break;
        case VertexDescription::AttributeType::INT:
            size = sizeof(int32_t) * attributes[i].size;
            switch (attributes[i].size) {
            case 1:
                vertexAttributes[i].format = WGPUVertexFormat_Uint32;
                break;
            case 2:
                vertexAttributes[i].format = WGPUVertexFormat_Uint32x2;
                break;
            case 3:
                vertexAttributes[i].format = WGPUVertexFormat_Uint32x3;
                break;
            case 4:
                vertexAttributes[i].format = WGPUVertexFormat_Uint32x4;
                break;
            default:
                assert(false);
            }
            break;
        case VertexDescription::AttributeType::FLOAT:
            size = sizeof(float) * attributes[i].size;
            switch (attributes[i].size) {
            case 1:
                vertexAttributes[i].format = WGPUVertexFormat_Float32;
                break;
            case 2:
                vertexAttributes[i].format = WGPUVertexFormat_Float32x2;
                break;
            case 3:
                vertexAttributes[i].format = WGPUVertexFormat_Float32x3;
                break;
            case 4:
                vertexAttributes[i].format = WGPUVertexFormat_Float32x4;
                break;
            default:
                assert(false);
            }
            break;
        default:
            assert(false);
        }

        vertexBufferLayout.arrayStride += size;
    }
}

WebGPUVertexDescription::~WebGPUVertexDescription() {
    delete[] vertexAttributes;
}

WGPUVertexBufferLayout WebGPUVertexDescription::GetVertexBufferLayout() const {
    return vertexBufferLayout;
}

} // namespace Video
