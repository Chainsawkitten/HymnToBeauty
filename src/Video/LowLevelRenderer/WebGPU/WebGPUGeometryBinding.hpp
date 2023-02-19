#pragma once

#include "../Interface/GeometryBinding.hpp"

namespace Video {

class Buffer;
class WebGPUBuffer;

/// WebGPU implementation of GeometryBinding.
class WebGPUGeometryBinding : public GeometryBinding {
  public:
    /// Create new WebGPU geometry binding.
    /**
     * @param vertexBuffer Buffer containing the vertex data.
     * @param indexType The type of values in the index buffer.
     * @param indexBuffer Index buffer.
     */
    WebGPUGeometryBinding(Buffer* vertexBuffer, IndexType indexType = IndexType::NONE, const Buffer* indexBuffer = nullptr);

    /// Destructor.
    ~WebGPUGeometryBinding() final;

    /// Get the vertex buffer.
    /**
     * @return The vertex buffer.
     */
    WebGPUBuffer* GetVertexBuffer();

    /// Get the type of values in the index buffer.
    /**
     * @return The type of values in the index buffer.
     */
    IndexType GetIndexType() const;

    /// Get the index buffer.
    /**
     * @return The index buffer.
     */
    const WebGPUBuffer* GetIndexBuffer() const;

  private:
    WebGPUGeometryBinding(const WebGPUGeometryBinding& other) = delete;

    WebGPUBuffer* vertexBuffer;
    const WebGPUBuffer* indexBuffer;
    IndexType indexType;
};

}
