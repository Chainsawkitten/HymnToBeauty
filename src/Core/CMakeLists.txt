set(SRCS
        GameWindow.cpp
        Util/FileSystem.cpp
        Util/Settings.cpp
    )

set(HEADERS
        GameWindow.hpp
        Util/FileSystem.hpp
        Util/Settings.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_library(Core STATIC ${SRCS} ${HEADERS})
target_compile_features(Core PRIVATE cxx_nullptr)
