set(SRCS
        game.cpp
    )

set(HEADERS
        game.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(Game ${SRCS} ${HEADERS})
target_link_libraries(Game Core)
target_link_libraries(Game glew glfw ${GLFW_LIBRARIES})
