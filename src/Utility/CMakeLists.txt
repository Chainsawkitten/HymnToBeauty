set(SRCS
        Log.cpp
    )

set(HEADERS
        Queue.hpp
        LockBox.hpp
        Log.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_library(Utility STATIC ${SRCS} ${HEADERS})
target_link_libraries(Utility glm)
set_property(TARGET Utility PROPERTY CXX_STANDARD 11)
set_property(TARGET Utility PROPERTY CXX_STANDARD_REQUIRED ON)
