# Assimp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries" FORCE)
set(ENABLE_BOOST_WORKAROUND ON CACHE BOOL "Building Assimp without boost" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Don't build Assimp tools" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Don't build Assimp view" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Don't build Assimp tests" FORCE)
set(UNUSED_MODEL_FORMATS
        ASSIMP_BUILD_3DS_IMPORTER
        ASSIMP_BUILD_3D_IMPORTER
        ASSIMP_BUILD_3MF_IMPORTER
        ASSIMP_BUILD_AC_IMPORTER
        ASSIMP_BUILD_AMF_IMPORTER
        ASSIMP_BUILD_ASE_IMPORTER
        ASSIMP_BUILD_ASSBIN_IMPORTER
        ASSIMP_BUILD_ASSXML_IMPORTER
        ASSIMP_BUILD_B3D_IMPORTER
        ASSIMP_BUILD_BLEND_IMPORTER
        ASSIMP_BUILD_BVH_IMPORTER
        ASSIMP_BUILD_COB_IMPORTER
        ASSIMP_BUILD_COLLADA_IMPORTER
        ASSIMP_BUILD_CSM_IMPORTER
        ASSIMP_BUILD_DXF_IMPORTER
        ASSIMP_BUILD_GLTF_IMPORTER
        ASSIMP_BUILD_HMP_IMPORTER
        ASSIMP_BUILD_IFC_IMPORTER
        ASSIMP_BUILD_IRRMESH_IMPORTER
        ASSIMP_BUILD_IRR_IMPORTER
        ASSIMP_BUILD_LWO_IMPORTER
        ASSIMP_BUILD_LWS_IMPORTER
        ASSIMP_BUILD_MD2_IMPORTER
        ASSIMP_BUILD_MD3_IMPORTER
        ASSIMP_BUILD_MD5_IMPORTER
        ASSIMP_BUILD_MDC_IMPORTER
        ASSIMP_BUILD_MDL_IMPORTER
        ASSIMP_BUILD_MS3D_IMPORTER
        ASSIMP_BUILD_NDO_IMPORTER
        ASSIMP_BUILD_NFF_IMPORTER
        ASSIMP_BUILD_OBJ_IMPORTER
        ASSIMP_BUILD_OFF_IMPORTER
        ASSIMP_BUILD_OGRE_IMPORTER
        ASSIMP_BUILD_OPENGEX_IMPORTER
        ASSIMP_BUILD_PLY_IMPORTER
        ASSIMP_BUILD_Q3BSP_IMPORTER
        ASSIMP_BUILD_Q3D_IMPORTER
        ASSIMP_BUILD_RAW_IMPORTER
        ASSIMP_BUILD_SIB_IMPORTER
        ASSIMP_BUILD_SMD_IMPORTER
        ASSIMP_BUILD_STL_IMPORTER
        ASSIMP_BUILD_TERRAGEN_IMPORTER
        ASSIMP_BUILD_X3D_IMPORTER
        ASSIMP_BUILD_XGL_IMPORTER
        ASSIMP_BUILD_X_IMPORTER
    )
foreach(model_format ${UNUSED_MODEL_FORMATS})
    set(${model_format} OFF CACHE BOOL "Don't build unused model formats" FORCE)
endforeach()
add_subdirectory(assimp)
target_include_directories(assimp INTERFACE assimp/include)

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
add_library(imgui STATIC imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp)
target_include_directories(imgui INTERFACE imgui)

# Imguizmo
add_library(imguizmo STATIC ImGuizmo/ImGuizmo.cpp)
target_link_libraries(imguizmo imgui)
target_include_directories(imguizmo INTERFACE ImGuizmo)

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

# ShaderProcessor
add_subdirectory(ShaderProcessor)
macro(ProcessShader headervar sourcevar inputfile outdir)
    if(VulkanRenderer)
        set(vulkanargument --vulkan)
    endif()
    get_filename_component(basefile ${inputfile} NAME)
    set(outputfile ${outdir}/${basefile})
    set_source_files_properties(${outputfile} PROPERTIES HEADER_ONLY TRUE)
    add_custom_command(
        OUTPUT ${outputfile}.hpp ${outputfile}.cpp
        COMMAND ShaderProcessor ${inputfile} ${outputfile} ${vulkanargument}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${inputfile}
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
