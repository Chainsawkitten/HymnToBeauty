#include "VulkanGeometryBinding.hpp"

#include <assert.h>
#include <Utility/Log.hpp>
#include "VulkanBuffer.hpp"

namespace Video {

VulkanGeometryBinding::VulkanGeometryBinding(Buffer* vertexBuffer, IndexType indexType, const Buffer* indexBuffer) {
    assert(vertexBuffer != nullptr);
    assert(indexType == IndexType::NONE || indexBuffer != nullptr);

    this->vertexBuffer = static_cast<VulkanBuffer*>(vertexBuffer);
    this->indexType = indexType;
    if (indexType != IndexType::NONE) {
        this->indexBuffer = static_cast<const VulkanBuffer*>(indexBuffer);
    } else {
        this->indexBuffer = nullptr;
    }
}

VulkanGeometryBinding::~VulkanGeometryBinding() {

}

VulkanBuffer* VulkanGeometryBinding::GetVertexBuffer() {
    return vertexBuffer;
}

GeometryBinding::IndexType VulkanGeometryBinding::GetIndexType() const {
    return indexType;
}

VkBuffer VulkanGeometryBinding::GetIndexBuffer() const {
    assert(indexType != IndexType::NONE);

    return indexBuffer->GetBuffer();
}

}
