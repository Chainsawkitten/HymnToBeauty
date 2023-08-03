# Includes (shader source)
add_library(ShaderSource INTERFACE)
target_include_directories(ShaderSource INTERFACE include)

if(CMAKE_CROSSCOMPILING)
    # Pawn off the creation of the host utilities into its own dedicated space
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools)
    file(TO_NATIVE_PATH ${CMAKE_COMMAND} native_cmake_command)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} native_cmake_current_source_dir)
    
    execute_process(
        COMMAND "${native_cmake_command}" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" "${native_cmake_current_source_dir}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools
    )

    add_custom_target(host_tools
        COMMAND ${CMAKE_COMMAND} --build . --target ShaderProcessor --config $<CONFIG>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools
    )
    include(${CMAKE_CURRENT_BINARY_DIR}/host_tools/host_tools.cmake)

    add_dependencies(ShaderProcessor host_tools)
    set_target_properties(ShaderProcessor PROPERTIES IMPORTED_GLOBAL TRUE)
else()
    # Source files
    set(SRCS
            src/main.cpp
            src/Shader.cpp
        )

    set(HEADERS
            src/Shader.hpp
        )

    if(COMMAND create_directory_groups)
        create_directory_groups(${SRCS} ${HEADERS})
    endif()

    add_executable(ShaderProcessor ${SRCS} ${HEADERS})
    target_link_libraries(ShaderProcessor ShaderSource ShaderLibraries)

    set_property(TARGET ShaderProcessor PROPERTY CXX_STANDARD 11)
    set_property(TARGET ShaderProcessor PROPERTY CXX_STANDARD_REQUIRED ON)
    
    export(TARGETS ShaderProcessor FILE host_tools.cmake)
endif()

# Macro to process shaders.
macro(ProcessShader headervar sourcevar inputfile outdir includes)
    if(VulkanRenderer)
        set(vulkanargument --vulkan)
    endif()
    if(WebGPURenderer)
        set(webgpuargument --webgpu)
    endif()
    get_filename_component(basefile ${inputfile} NAME)
    set(outputfile ${outdir}/${basefile})
    set_source_files_properties(${outputfile}.hpp PROPERTIES HEADER_ONLY TRUE)
    add_custom_command(
        OUTPUT ${outputfile}.hpp ${outputfile}.cpp
        COMMAND ShaderProcessor ${inputfile} ${outputfile} ${vulkanargument} ${webgpuargument}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${inputfile} ${includes}
        COMMENT "Generating shader variants for ${inputfile}"
    )
    list(APPEND ${headervar} ${outputfile}.hpp)
    list(APPEND ${sourcevar} ${outputfile}.cpp)
endmacro()
