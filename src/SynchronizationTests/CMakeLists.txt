set(SRCS
        Framework/Meshes.cpp
        Framework/Runner.cpp
        Framework/SynchronizationTest.cpp
        Tests/ExampleTest.cpp
        Tests/FragFragBuffer.cpp
        Tests/FragFragImage.cpp
        Tests/FragVertImage.cpp
    )

set(HEADERS
        Framework/Meshes.hpp
        Framework/Runner.hpp
        Framework/SynchronizationTest.hpp
        Tests/ExampleTest.hpp
        Tests/FragFragBuffer.hpp
        Tests/FragFragImage.hpp
        Tests/FragVertImage.hpp
    )
        
create_directory_groups(${SRCS} ${HEADERS})

# Setup shaders
set(SHADER_INCLUDES
    Shaders/Lighting.glsl
)

set(SHADERS
    Shaders/Blur.frag
    Shaders/Cube.vert
    Shaders/Cube.frag
    Shaders/CubeTiled.frag
    Shaders/LightTiling.vert
    Shaders/LightTiling.frag
    Shaders/Offset.vert
    Shaders/Offset.frag
    Shaders/Plane.vert
    Shaders/Plane.frag
    Shaders/PostProcessing.vert
)

# Process shaders.
foreach(item ${SHADERS})
    ProcessShader(SHADER_HEADER SHADER_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR} "")
endforeach()

if (ANDROID)
    # Build as a library to include in an Android app.
    add_library(synchronizationtests
            SHARED
            AndroidMain.cpp
            GameActivitySources.cpp
            ${SRCS}
            ${HEADERS}
            ${SHADER_SOURCE}
            ${SHADER_HEADER}
        )

    target_link_libraries(synchronizationtests Video)
else()
    # Build as an executable
    add_executable(SynchronizationTests
            main.cpp
            ${SRCS}
            ${HEADERS}
            ${SHADER_SOURCE}
            ${SHADER_HEADER}
        )
    target_link_libraries(SynchronizationTests Video)

    set_property(TARGET SynchronizationTests PROPERTY CXX_STANDARD 11)
    set_property(TARGET SynchronizationTests PROPERTY CXX_STANDARD_REQUIRED ON)
endif()