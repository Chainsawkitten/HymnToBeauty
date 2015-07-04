set(SRCS
        GameWindow.cpp
        Util/FileSystem.cpp
        Util/Settings.cpp
        Util/Log.cpp
        Shader/Shader.cpp
        Shader/ShaderProgram.cpp
        Geometry/Geometry2D.cpp
        Geometry/Rectangle.cpp
    )

set(HEADERS
        GameWindow.hpp
        Util/FileSystem.hpp
        Util/Settings.hpp
        Util/Log.hpp
        Shader/Shader.hpp
        Shader/ShaderProgram.hpp
        Geometry/Geometry2D.hpp
        Geometry/Rectangle.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_library(Core STATIC ${SRCS} ${HEADERS})
target_compile_features(Core PRIVATE cxx_nullptr)
