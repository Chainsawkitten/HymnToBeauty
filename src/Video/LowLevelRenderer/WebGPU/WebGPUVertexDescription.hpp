#pragma once

#include "../Interface/VertexDescription.hpp"

#include <dawn/webgpu.h>

namespace Video {

/// WebGPU implementation of VertexDescription.
class WebGPUVertexDescription : public VertexDescription {
  public:
    /// Create new WebGPU vertex description.
    /**
     * @param attributeCount The number of vertex attributes.
     * @param attributes The array of attributes.
     *
     * @return The created vertex description.
     */
    WebGPUVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes);

    /// Destructor.
    ~WebGPUVertexDescription() final;

    /// Get the vertex buffer layout to use in graphics pipeines.
    /**
     * @return The vertex buffer layout.
     */
    WGPUVertexBufferLayout GetVertexBufferLayout() const;

  private:
    WebGPUVertexDescription(const WebGPUVertexDescription& other) = delete;

    WGPUVertexBufferLayout vertexBufferLayout;
    WGPUVertexAttribute* vertexAttributes;
};

}
