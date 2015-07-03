set(SRCS
        GameWindow.cpp
    )

set(HEADERS
        GameWindow.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_library(Core STATIC ${SRCS} ${HEADERS})
target_compile_features(Core PRIVATE cxx_nullptr)
