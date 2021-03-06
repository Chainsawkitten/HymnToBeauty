cmake_minimum_required(VERSION 3.7.0)

project(HymnToBeauty)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(UNIX OR MINGW)
    option(HTB_DEBUG_INFO "Build Hymn to Beauty with DWARF debug data." OFF)
    if(HTB_DEBUG_INFO)
        set(DEBUG_DATA "-gdwarf-2")
    else()
        set(DEBUG_DATA "-s")
    endif()
    
    set(CMAKE_CXX_FLAGS "-g ${DEBUG_DATA} -O3 -static-libstdc++ -Wall -fno-strict-aliasing")
elseif(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

option(OpenGLRenderer "OpenGL Renderer" ON)
option(VulkanRenderer "Vulkan Renderer" OFF)

# Externals
add_subdirectory(externals)

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

option(UseDoubleLogging "Log messages will be duplicated and sent to standard out." ON)
if(UseDoubleLogging)
    add_definitions(-DUSINGDOUBLELOGGING)
endif()

# Source files
add_subdirectory(src)

# Tests
option(HTB_TESTS "Build tests" ON)
if (HTB_TESTS)
    add_subdirectory(tests)
endif()
