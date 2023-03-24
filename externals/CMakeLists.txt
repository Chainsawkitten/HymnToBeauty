# AngelScript
add_subdirectory(AngelScript/sdk/angelscript/projects/cmake)
add_library(AngelScriptSDK INTERFACE)
target_link_libraries(AngelScriptSDK INTERFACE angelscript)
target_include_directories(AngelScriptSDK INTERFACE AngelScript/sdk/angelscript/include)
target_include_directories(AngelScriptSDK INTERFACE AngelScript/sdk/add_on)

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
if (NOT ANDROID)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build GLFW docs" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build GLFW examples" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build GLFW tests" FORCE)
    add_subdirectory(glfw)
endif()

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
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build JsonCpp with shared libs" FORCE)
set(BUILD_OBJECT_LIBS OFF CACHE BOOL "Don't build JsonCpp with object libs" FORCE)
add_subdirectory(jsoncpp)

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

# miniaudio
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE miniaudio)

# STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

if (WebGPURenderer)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/dawn")
        message(VERBOSE "Using dawn as WebGPU backend")
        add_subdirectory(dawn)
        set(WEBGPU_BACKEND_DAWN ON PARENT_SCOPE)
    elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/wgpu-native")
        message(VERBOSE "Using wgpu as WebGPU backend")
        set(WEBGPU_BACKEND_WGPU ON PARENT_SCOPE)
        
        # Find library
        if (ANDROID)
            find_library(WGPU_LIBRARY wgpu_native
                HINTS "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/target/aarch64-linux-android/release"
            )
        else()
            find_library(WGPU_LIBRARY wgpu_native
                HINTS "${CMAKE_CURRENT_SOURCE_DIR}/wgpu-native/target/release"
            )
        endif()
        
        # Setup WebGPU headers.
        add_library(wgpu_headers INTERFACE)
        target_include_directories(wgpu_headers INTERFACE wgpu-native/ffi)
        
        # Dependencies
        if(MSVC)
            set(WGPU_OS_LIBRARIES "userenv" "ws2_32" "Dwmapi" "dbghelp" "d3dcompiler" "D3D12" "D3D11" "DXGI" "setupapi" "Bcrypt" PARENT_SCOPE)
        elseif(APPLE)
            set(WGPU_OS_LIBRARIES "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore" PARENT_SCOPE)
        endif()
    else()
        message(FATAL_ERROR "WebGPURenderer requested but no WebGPU backend found")
    endif()
endif()
