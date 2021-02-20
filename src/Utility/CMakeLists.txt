set(SRCS
        Log.cpp
        Profiling/Event.cpp
        Profiling/Thread.cpp
        Profiling/Timeline.cpp
    )

set(HEADERS
        Queue.hpp
        LockBox.hpp
        Log.hpp
        Profiling/Event.hpp
        Profiling/Thread.hpp
        Profiling/Timeline.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_library(Utility STATIC ${SRCS} ${HEADERS})
target_link_libraries(Utility glm jsoncpp_lib_static)
set_property(TARGET Utility PROPERTY CXX_STANDARD 11)
set_property(TARGET Utility PROPERTY CXX_STANDARD_REQUIRED ON)
