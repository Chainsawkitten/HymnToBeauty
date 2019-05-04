#pragma once

#include "Types.hpp"

namespace Video {

/// Binds together a vertex description with buffers.
class GeometryBinding {
  public:
    /// The type of values in the index buffer.
    enum class IndexType {
        NONE = 0, ///< No index buffer.
        SHORT, ///< 16 bits per index.
        INT ///< 32 bits per index.
    };

    /// Create a new shader.
    GeometryBinding() {}

    /// Destructor.
    virtual ~GeometryBinding() {}

  private:
    GeometryBinding(const GeometryBinding& other) = delete;
};

}
