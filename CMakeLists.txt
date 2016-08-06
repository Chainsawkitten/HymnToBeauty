cmake_minimum_required(VERSION 2.8.12)

project(HymnToBeauty)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(UNIX OR MINGW)
    set(CMAKE_CXX_FLAGS "-g -gdwarf-2 -static-libstdc++ -Wall")
endif()

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
    shaders/Default3D.vert
    shaders/Default3D.frag
    shaders/Deferred.frag
    shaders/Font.vert
    shaders/Font.frag
    shaders/SingleColor2D.frag
    shaders/Texture2D.frag
    shaders/Post.vert
    shaders/PostCopy.frag
    assets/ABeeZee.ttf
    assets/File.png
    assets/Options.png
    assets/Play.png
    assets/Close.png
    assets/NewHymn.png
    assets/OpenHymn.png
    assets/Add.png
    assets/Directory.png
    assets/BasicFile.png
)

# Headerize
add_subdirectory(externals/Headerize)
macro(ProcessWithHeaderize headervar sourcevar inputfile outdir)
    get_filename_component(basefile ${inputfile} NAME)
    set(outputfile ${outdir}/${basefile})
    set_source_files_properties(${outputfile} PROPERTIES HEADER_ONLY TRUE)
    add_custom_command(
        OUTPUT ${outputfile}.hpp ${outputfile}.cpp
        COMMAND Headerize ${inputfile} ${outputfile}
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

# Doxygen documentation
option(HymnToBeautyBuildDocs "Build the doxygen documentation" OFF)
find_package(Doxygen)
if(DOXYGEN_FOUND AND HymnToBeautyBuildDocs)
    add_custom_target(EngineDoc ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/EngineDoxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
    add_custom_target(EditorDoc ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/EditorDoxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
    add_custom_target(GameDoc ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/GameDoxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()

include_directories(src)


## TARGETS ##

## Engine ##
set(ENGINE_SRCS
        src/Engine/GameWindow.cpp
        src/Engine/Hymn.cpp
        src/Engine/RenderTarget.cpp
        src/Engine/Component/Lens.cpp
        src/Engine/Component/Mesh.cpp
        src/Engine/Component/SuperComponent.cpp
        src/Engine/Component/Transform.cpp
        src/Engine/Entity/Entity.cpp
        src/Engine/Font/Font.cpp
        src/Engine/Lighting/DeferredLighting.cpp
        src/Engine/Manager/Managers.cpp
        src/Engine/Manager/RenderManager.cpp
        src/Engine/Manager/ResourceManager.cpp
        src/Engine/Physics/Rectangle.cpp
        src/Engine/Util/FileSystem.cpp
        src/Engine/Util/Input.cpp
        src/Engine/Util/Settings.cpp
        src/Engine/Util/Log.cpp
        src/Engine/Shader/Shader.cpp
        src/Engine/Shader/ShaderProgram.cpp
        src/Engine/Geometry/Cube.cpp
        src/Engine/Geometry/Geometry2D.cpp
        src/Engine/Geometry/Geometry3D.cpp
        src/Engine/Geometry/OBJModel.cpp
        src/Engine/Geometry/Rectangle.cpp
        src/Engine/Scene/Scene.cpp
        src/Engine/Texture/Texture.cpp
        src/Engine/Texture/Texture2D.cpp
    )

set(ENGINE_HEADERS
        src/Engine/GameWindow.hpp
        src/Engine/Hymn.hpp
        src/Engine/RenderTarget.hpp
        src/Engine/Component/Lens.hpp
        src/Engine/Component/Mesh.hpp
        src/Engine/Component/SuperComponent.hpp
        src/Engine/Component/Transform.hpp
        src/Engine/Entity/Entity.hpp
        src/Engine/Font/Font.hpp
        src/Engine/Lighting/DeferredLighting.hpp
        src/Engine/Manager/Managers.hpp
        src/Engine/Manager/RenderManager.hpp
        src/Engine/Manager/ResourceManager.hpp
        src/Engine/Physics/Rectangle.hpp
        src/Engine/Util/FileSystem.hpp
        src/Engine/Util/Input.hpp
        src/Engine/Util/Settings.hpp
        src/Engine/Util/Log.hpp
        src/Engine/Shader/Shader.hpp
        src/Engine/Shader/ShaderProgram.hpp
        src/Engine/Geometry/Cube.hpp
        src/Engine/Geometry/Geometry2D.hpp
        src/Engine/Geometry/Geometry3D.hpp
        src/Engine/Geometry/OBJModel.hpp
        src/Engine/Geometry/Rectangle.hpp
        src/Engine/Scene/Scene.hpp
        src/Engine/Texture/Texture.hpp
        src/Engine/Texture/Texture2D.hpp
    )

create_directory_groups(${ENGINE_SRCS} ${ENGINE_HEADERS})

set_property(SOURCE ${ENGINE_SRCS} APPEND PROPERTY OBJECT_DEPENDS ${EMBEDDED_HEADER})

add_library(Engine STATIC ${ENGINE_SRCS} ${ENGINE_HEADERS} ${EMBEDDED_SOURCE})
set_property(TARGET Engine PROPERTY CXX_STANDARD 11)
set_property(TARGET Engine PROPERTY CXX_STANDARD_REQUIRED ON)



## Editor ##
set(EDITOR_SRCS
        src/Editor/editor.cpp
        src/Editor/EditorWindow.cpp
        src/Editor/GUI/Widget.cpp
        src/Editor/GUI/Container.cpp
        src/Editor/GUI/HorizontalLayout.cpp
        src/Editor/GUI/VerticalLayout.cpp
        src/Editor/GUI/VerticalScrollLayout.cpp
        src/Editor/GUI/Button.cpp
        src/Editor/GUI/ImageButton.cpp
        src/Editor/GUI/ImageTextButton.cpp
        src/Editor/GUI/TextButton.cpp
        src/Editor/GUI/SelectHymnWindow.cpp
        src/Editor/GUI/FileSelector.cpp
        src/Editor/GUI/TextField.cpp
        src/Editor/GUI/Label.cpp
        src/Editor/GUI/ResourceList.cpp
        src/Editor/GUI/EntityEditor.cpp
        src/Editor/GUI/ModelEditor.cpp
        src/Editor/GUI/ComponentEditor/ComponentEditor.cpp
        src/Editor/GUI/ComponentEditor/LensEditor.cpp
        src/Editor/GUI/ComponentEditor/MeshEditor.cpp
        src/Editor/GUI/ComponentEditor/TransformEditor.cpp
        src/Editor/GUI/FloatEditor.cpp
        src/Editor/GUI/Vec3Editor.cpp
        src/Editor/GUI/StringEditor.cpp
        src/Editor/Util/EditorSettings.cpp
    )

set(EDITOR_HEADERS
        src/Editor/editor.hpp
        src/Editor/EditorWindow.hpp
        src/Editor/GUI/Widget.hpp
        src/Editor/GUI/Container.hpp
        src/Editor/GUI/HorizontalLayout.hpp
        src/Editor/GUI/VerticalLayout.hpp
        src/Editor/GUI/VerticalScrollLayout.hpp
        src/Editor/GUI/Button.hpp
        src/Editor/GUI/ImageButton.hpp
        src/Editor/GUI/ImageTextButton.hpp
        src/Editor/GUI/TextButton.hpp
        src/Editor/GUI/SelectHymnWindow.hpp
        src/Editor/GUI/FileSelector.hpp
        src/Editor/GUI/TextField.hpp
        src/Editor/GUI/Label.hpp
        src/Editor/GUI/ResourceList.hpp
        src/Editor/GUI/EntityEditor.hpp
        src/Editor/GUI/ModelEditor.hpp
        src/Editor/GUI/ComponentEditor/ComponentEditor.hpp
        src/Editor/GUI/ComponentEditor/LensEditor.hpp
        src/Editor/GUI/ComponentEditor/MeshEditor.hpp
        src/Editor/GUI/ComponentEditor/TransformEditor.hpp
        src/Editor/GUI/FloatEditor.hpp
        src/Editor/GUI/Vec3Editor.hpp
        src/Editor/GUI/StringEditor.cpp
        src/Editor/Util/EditorSettings.hpp
    )

create_directory_groups(${EDITOR_SRCS} ${EDITOR_HEADERS})

add_executable(HymnToBeauty ${EDITOR_SRCS} ${EDITOR_HEADERS})
target_link_libraries(HymnToBeauty Engine)
target_link_libraries(HymnToBeauty glew glfw ${GLFW_LIBRARIES})
set_property(TARGET HymnToBeauty PROPERTY CXX_STANDARD 11)
set_property(TARGET HymnToBeauty PROPERTY CXX_STANDARD_REQUIRED ON)



## Game ##
set(GAME_SRCS
        src/Game/game.cpp
    )

set(GAME_HEADERS
        src/Game/game.hpp
    )

create_directory_groups(${GAME_SRCS} ${GAME_HEADERS})

add_executable(Game ${GAME_SRCS} ${GAME_HEADERS})
target_link_libraries(Game Engine)
target_link_libraries(Game glew glfw ${GLFW_LIBRARIES})
set_property(TARGET Game PROPERTY CXX_STANDARD 11)
set_property(TARGET Game PROPERTY CXX_STANDARD_REQUIRED ON)
