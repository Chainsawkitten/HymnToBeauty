#pragma once

#include "Types.hpp"

namespace Video {

/// A compute pipeline.
/**
 * @todo Write better documentation.
 */
class ComputePipeline {
  public:
    /// Create a new compute pipeline.
    ComputePipeline() {}

    /// Destructor.
    virtual ~ComputePipeline() {}

  private:
    ComputePipeline(const ComputePipeline& other) = delete;
};

}
