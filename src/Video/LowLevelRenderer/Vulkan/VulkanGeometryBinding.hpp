#pragma once

#include "../Interface/GeometryBinding.hpp"

#include <vulkan/vulkan.h>

namespace Video {

class Buffer;
class VulkanBuffer;

/// Vulkan implementation of GeometryBinding.
class VulkanGeometryBinding : public GeometryBinding {
  public:
    /// Create new Vulkan geometry binding.
    /**
     * @param vertexBuffer Buffer containing the vertex data.
     * @param indexType The type of values in the index buffer.
     * @param indexBuffer Index buffer.
     */
    VulkanGeometryBinding(Buffer* vertexBuffer, IndexType indexType = IndexType::NONE, const Buffer* indexBuffer = nullptr);

    /// Destructor.
    ~VulkanGeometryBinding() final;

    /// Get the vertex buffer.
    /**
     * @return The vertex buffer.
     */
    VulkanBuffer* GetVertexBuffer();

    /// Get the type of values in the index buffer.
    /**
     * @return The type of values in the index buffer.
     */
    IndexType GetIndexType() const;

    /// Get the index buffer.
    /**
     * @return The index buffer.
     */
    const VulkanBuffer* GetIndexBuffer() const;

  private:
    VulkanGeometryBinding(const VulkanGeometryBinding& other) = delete;

    VulkanBuffer* vertexBuffer;
    const VulkanBuffer* indexBuffer;
    IndexType indexType;
};

}
