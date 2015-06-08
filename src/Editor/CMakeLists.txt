set(SRCS
        main.cpp
    )

set(HEADERS

    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(HymnToBeauty ${SRCS} ${HEADERS})
target_link_libraries(HymnToBeauty glfw ${GLFW_LIBRARIES})
