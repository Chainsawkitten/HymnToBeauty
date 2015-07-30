cmake_minimum_required(VERSION 2.8.12)

project(HymnToBeauty)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests" FORCE)
add_subdirectory(externals/glfw)
include_directories(externals/glfw/include)

# GLEW
add_subdirectory(externals/glew/build/cmake)
include_directories(externals/glew/include)

# SimpleIni
include_directories(externals/simpleini)

# GLM
include_directories(externals/glm)

# STB
include_directories(externals/stb)

# This function should be passed a list of all files in a target. It will automatically generate
# file groups following the directory hierarchy, so that the layout of the files in IDEs matches the
# one in the filesystem.
function(create_directory_groups)
    # Place any files that aren't in the source list in a separate group so that they don't get in
    # the way.
    source_group("Other Files" REGULAR_EXPRESSION ".")

    foreach(file_name ${ARGV})
        get_filename_component(dir_name "${file_name}" PATH)
        # Group names use '\' as a separator even though the entire rest of CMake uses '/'...
        string(REPLACE "/" "\\" group_name "${dir_name}")
        source_group("${group_name}" FILES "${file_name}")
    endforeach()
endfunction()

# Setup embedded source
set(EMBEDDED_INPUT
    shaders/Default2D.vert
    shaders/SingleColor2D.frag
    shaders/Texture2D.frag
    assets/File.png
    assets/Options.png
    assets/Play.png
)

# Headerize
find_program(HEADERIZE_EXE Headerize
    DOC "Location of the Headerize executable"
)

macro(ProcessWithHeaderize headervar sourcevar inputfile outdir)
    get_filename_component(basefile ${inputfile} NAME)
    set(outputfile ${outdir}/${basefile})
    set_source_files_properties(${outputfile} PROPERTIES HEADER_ONLY TRUE)
    add_custom_command(
        OUTPUT ${outputfile}.hpp ${outputfile}.cpp
        COMMAND ${HEADERIZE_EXE} ${inputfile} ${outputfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${inputfile}
        COMMENT "Generating headers for ${inputfile}"
    )
    list(APPEND ${headervar} ${outputfile}.hpp)
    list(APPEND ${sourcevar} ${outputfile}.cpp)
endmacro()

foreach(item ${EMBEDDED_INPUT})
    ProcessWithHeaderize(EMBEDDED_HEADER EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

include_directories(src)


## TARGETS ##

## Core ##
set(CORE_SRCS
        src/Core/GameWindow.cpp
        src/Core/Resources.cpp
        src/Core/Util/FileSystem.cpp
        src/Core/Util/Input.cpp
        src/Core/Util/Settings.cpp
        src/Core/Util/Log.cpp
        src/Core/Shader/Shader.cpp
        src/Core/Shader/ShaderProgram.cpp
        src/Core/Geometry/Geometry2D.cpp
        src/Core/Geometry/Rectangle.cpp
        src/Core/GUI/Widget.cpp
        src/Core/GUI/Container.cpp
        src/Core/GUI/HorizontalLayout.cpp
        src/Core/GUI/VerticalLayout.cpp
        src/Core/GUI/Button.cpp
        src/Core/Texture/Texture.cpp
        src/Core/Texture/Texture2D.cpp
    )

set(CORE_HEADERS
        src/Core/GameWindow.hpp
        src/Core/Resources.hpp
        src/Core/Util/FileSystem.hpp
        src/Core/Util/Input.hpp
        src/Core/Util/Settings.hpp
        src/Core/Util/Log.hpp
        src/Core/Shader/Shader.hpp
        src/Core/Shader/ShaderProgram.hpp
        src/Core/Geometry/Geometry2D.hpp
        src/Core/Geometry/Rectangle.hpp
        src/Core/GUI/Widget.hpp
        src/Core/GUI/Container.hpp
        src/Core/GUI/HorizontalLayout.hpp
        src/Core/GUI/VerticalLayout.hpp
        src/Core/GUI/Button.hpp
        src/Core/Texture/Texture.hpp
        src/Core/Texture/Texture2D.hpp
    )

create_directory_groups(${CORE_SRCS} ${CORE_HEADERS})

set_property(SOURCE ${CORE_SRCS} APPEND PROPERTY OBJECT_DEPENDS ${EMBEDDED_HEADER})

add_library(Core STATIC ${CORE_SRCS} ${CORE_HEADERS} ${EMBEDDED_SOURCE})
target_compile_features(Core PRIVATE cxx_nullptr)



## Editor ##
set(EDITOR_SRCS
        src/Editor/editor.cpp
        src/Editor/EditorWindow.cpp
        src/Editor/Util/EditorSettings.cpp
    )

set(EDITOR_HEADERS
        src/Editor/editor.hpp
        src/Editor/EditorWindow.hpp
        src/Editor/Util/EditorSettings.hpp
    )

create_directory_groups(${EDITOR_SRCS} ${EDITOR_HEADERS})

add_executable(HymnToBeauty ${EDITOR_SRCS} ${EDITOR_HEADERS})
target_link_libraries(HymnToBeauty Core)
target_link_libraries(HymnToBeauty glew glfw ${GLFW_LIBRARIES})
target_compile_features(HymnToBeauty PRIVATE cxx_nullptr cxx_deleted_functions)



## Game ##
set(GAME_SRCS
        src/Game/game.cpp
    )

set(GAME_HEADERS
        src/Game/game.hpp
    )

create_directory_groups(${GAME_SRCS} ${GAME_HEADERS})

add_executable(Game ${GAME_SRCS} ${GAME_HEADERS})
target_link_libraries(Game Core)
target_link_libraries(Game glew glfw ${GLFW_LIBRARIES})
