cmake_minimum_required(VERSION 3.7.0)

project(ShaderProcessor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

# Includes (shader source)
add_library(ShaderSource INTERFACE)
target_include_directories(ShaderSource INTERFACE include)

# Source files
set(SRCS
        src/main.cpp
        src/Shader.cpp
    )

set(HEADERS
        src/Shader.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(ShaderProcessor ${SRCS} ${HEADERS})
target_link_libraries(ShaderProcessor ShaderSource)

set_property(TARGET ShaderProcessor PROPERTY CXX_STANDARD 11)
set_property(TARGET ShaderProcessor PROPERTY CXX_STANDARD_REQUIRED ON)
