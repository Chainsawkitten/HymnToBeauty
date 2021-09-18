# AngelScript
add_subdirectory(AngelScript/sdk/angelscript/projects/cmake)
target_include_directories(angelscript INTERFACE AngelScript/sdk/angelscript/include)
target_include_directories(angelscript INTERFACE AngelScript/sdk/add_on)

# Bullet
set(BUILD_CPU_DEMOS OFF CACHE BOOL "Don't build bullet demos." FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Don't build bullet tests." FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "Don't build bullet extras." FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Don't build bullet 2 demos." FORCE)
set(BUILD_BULLET3 OFF CACHE BOOL "Don't build bullet 3." FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Build using /MD with MSVC." FORCE)
add_subdirectory(bullet3)
add_library(bullet INTERFACE)
target_link_libraries(bullet INTERFACE BulletDynamics BulletCollision LinearMath)
target_include_directories(bullet INTERFACE bullet3/src)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW docs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests" FORCE)
add_subdirectory(glfw)
#target_include_directories(glfw INTERFACE glfw/include)

# glad
add_subdirectory(glad)

# SimpleIni
add_library(simpleini INTERFACE)
target_include_directories(simpleini INTERFACE simpleini)

# GLM
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

# Imgui
add_library(imgui STATIC imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)
target_include_directories(imgui INTERFACE imgui)

# Imguizmo
add_library(imguizmo STATIC ImGuizmo/ImGuizmo.cpp)
target_link_libraries(imguizmo imgui)
target_include_directories(imguizmo INTERFACE ImGuizmo)
set_property(TARGET imguizmo PROPERTY CXX_STANDARD 11)
set_property(TARGET imguizmo PROPERTY CXX_STANDARD_REQUIRED ON)

# JsonCpp
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Don't build JsonCpp tests" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Don't build JsonCpp unit tests" FORCE)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "Don't build JsonCpp with pkgconfig support" FORCE)
add_subdirectory(jsoncpp)
target_include_directories(jsoncpp_lib_static INTERFACE jsoncpp/include)

# Headerize
add_subdirectory(Headerize)
macro(ProcessWithHeaderize headervar sourcevar inputfile outdir)
    get_filename_component(basefile ${inputfile} NAME)
    set(outputfile ${outdir}/${basefile})
    set_source_files_properties(${outputfile}.hpp PROPERTIES HEADER_ONLY TRUE)
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

# ShaderProcessor
add_subdirectory(ShaderProcessor)
macro(ProcessShader headervar sourcevar inputfile outdir includes)
    if(VulkanRenderer)
        set(vulkanargument --vulkan)
    endif()
    get_filename_component(basefile ${inputfile} NAME)
    set(outputfile ${outdir}/${basefile})
    set_source_files_properties(${outputfile}.hpp PROPERTIES HEADER_ONLY TRUE)
    add_custom_command(
        OUTPUT ${outputfile}.hpp ${outputfile}.cpp
        COMMAND ShaderProcessor ${inputfile} ${outputfile} ${vulkanargument}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${inputfile} ${includes}
        COMMENT "Generating shader variants for ${inputfile}"
    )
    list(APPEND ${headervar} ${outputfile}.hpp)
    list(APPEND ${sourcevar} ${outputfile}.cpp)
endmacro()

# OpenAL Soft
set(ALSOFT_UTILS OFF CACHE BOOL "Don't build OpenAL Soft utility programs" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Don't build OpenAL Soft examples" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "Don't build OpenAL Soft tests" FORCE)
add_subdirectory(openal-soft)
add_library(OpenALSoft INTERFACE)
target_link_libraries(OpenALSoft INTERFACE OpenAL)
target_include_directories(OpenALSoft INTERFACE openal-soft/include)

# STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
