#pragma once

#include <glad/glad.h>
#include <cstdint>

namespace Video {
/// Queries information from the GPU.
class Query {
  public:
    /// Query types.
    enum Type {
        TIME_ELAPSED,  ///< Record the time in nanoseconds(ns) that it takes for the GPU to execute all of the scoped commands between Begin() and End().
        SAMPLES_PASSED ///< Record the number of samples that pass the depth test for all drawing commands between Begin() and End().
    };

    /// Create new query.
    /**
     * @param type Specifies the type of query object.
     */
    explicit Query(Type type);

    /// Destructor.
    ~Query();

    /// Start query.
    void Begin();

    /// Stop query.
    void End();

    /// Get query type.
    /**
     * @return The type of query.
     */
    Type GetType() const;

    /// Resolve query result.
    /**
     * @return The result of query.
     */
    std::uint64_t Resolve() const;

  private:
    Query(const Query& other) = delete;

    Type type;

    bool active;

    int queryCount;
    GLuint* queries;
    GLuint64* results;
    GLenum target;
};
} // namespace Video
