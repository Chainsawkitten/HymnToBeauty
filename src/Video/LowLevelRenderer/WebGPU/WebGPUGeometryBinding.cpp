#include "WebGPUGeometryBinding.hpp"

#include <cassert>
#include <Utility/Log.hpp>
#include "WebGPUBuffer.hpp"

namespace Video {

WebGPUGeometryBinding::WebGPUGeometryBinding(Buffer* vertexBuffer, IndexType indexType, const Buffer* indexBuffer) {
    assert(vertexBuffer != nullptr);
    assert(indexType == IndexType::NONE || indexBuffer != nullptr);

    this->vertexBuffer = static_cast<WebGPUBuffer*>(vertexBuffer);
    this->indexType = indexType;
    if (indexType != IndexType::NONE) {
        this->indexBuffer = static_cast<const WebGPUBuffer*>(indexBuffer);
    } else {
        this->indexBuffer = nullptr;
    }
}

WebGPUGeometryBinding::~WebGPUGeometryBinding() {

}

WebGPUBuffer* WebGPUGeometryBinding::GetVertexBuffer() {
    return vertexBuffer;
}

GeometryBinding::IndexType WebGPUGeometryBinding::GetIndexType() const {
    return indexType;
}

const WebGPUBuffer* WebGPUGeometryBinding::GetIndexBuffer() const {
    assert(indexType != IndexType::NONE);

    return indexBuffer;
}

}
