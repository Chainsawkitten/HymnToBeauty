#include "Query.hpp"

#include <GL/glew.h>
#include <Utility/Log.hpp>

using namespace Video;

Query::Query(Type type) : active(false) {
    this->type = type;
    switch (type) {
        case TIME_ELAPSED:
            target = GL_TIMESTAMP;
            break;
        case SAMPLES_PASSED:
            target = GL_SAMPLES_PASSED;
            break;
        default:
            assert(false);
    }

    if (type == TIME_ELAPSED)
        queryCount = 2;
    else
        queryCount = 1;

    queries = new GLuint[queryCount];
    results = new GLuint64[queryCount];
    glGenQueries(queryCount, queries);
}

Query::~Query() {
    glDeleteQueries(queryCount, queries);
    delete[] queries;
    delete[] results;
}

void Query::Begin() {
    if (active) {
        Log() << "Query::Begin warning: Already active.\n";
        return;
    }

    if (type == TIME_ELAPSED)
        glQueryCounter(queries[0], target);
    else
        glBeginQuery(target, queries[0]);

    active = true;
}

void Query::End() {
    if (!active) {
        Log() << "Query::End warning: Not active.\n";
        return;
    }

    if (type == TIME_ELAPSED)
        glQueryCounter(queries[1], target);
    else
        glEndQuery(target);

    active = false;
}

Query::Type Query::GetType() const {
    return type;
}

std::uint64_t Query::Resolve() const {
    if (active) {
        Log() << "Query::Resolve warning: Can't resolve query while active.\n";
        return 0;
    }

    for (int i = 0; i < queryCount; ++i) {
        GLuint64 available = GL_FALSE;
        while (available == GL_FALSE)
            glGetQueryObjectui64v(queries[i], GL_QUERY_RESULT_AVAILABLE, &available);
        glGetQueryObjectui64v(queries[i], GL_QUERY_RESULT, &results[i]);
    }

    if (type == TIME_ELAPSED)
        return results[1] - results[0];
    else
        return results[0];
}
