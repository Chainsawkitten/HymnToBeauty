set(SRCS
        editor.cpp
        EditorWindow.cpp
    )

set(HEADERS
        editor.hpp
        EditorWindow.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(HymnToBeauty ${SRCS} ${HEADERS})
target_link_libraries(HymnToBeauty Core)
target_link_libraries(HymnToBeauty glew glfw ${GLFW_LIBRARIES})
target_compile_features(HymnToBeauty PRIVATE cxx_nullptr)
