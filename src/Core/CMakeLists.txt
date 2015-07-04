set(SRCS
        GameWindow.cpp
        Util/FileSystem.cpp
        Util/Settings.cpp
        Util/Log.cpp
        Shader/Shader.cpp
    )

set(HEADERS
        GameWindow.hpp
        Util/FileSystem.hpp
        Util/Settings.hpp
        Util/Log.hpp
        Shader/Shader.cpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_library(Core STATIC ${SRCS} ${HEADERS})
target_compile_features(Core PRIVATE cxx_nullptr)
