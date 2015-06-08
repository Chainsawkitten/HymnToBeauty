set(SRCS
        editor.cpp
    )

set(HEADERS
        editor.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(HymnToBeauty ${SRCS} ${HEADERS})
target_link_libraries(HymnToBeauty glfw ${GLFW_LIBRARIES})
