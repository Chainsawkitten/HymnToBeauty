set(SRCS
        main.cpp
    )

set(HEADERS

    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(Game ${SRCS} ${HEADERS})
target_link_libraries(Game glfw ${GLFW_LIBRARIES})
