set(SRCS
    main.cpp
    Framework/TestSuite.cpp
    Video/ComputeTests.cpp
    Video/DrawTests.cpp
    Video/ImageVerification.cpp
    Video/SwapChainTests.cpp
    Video/VideoSuite.cpp
)

set(HEADERS
    Framework/TestSuite.hpp
    Video/ImageVerification.hpp
    Video/VideoSuite.hpp
)

# Setup shaders
set(SHADERS
    Video/shaders/ComputeSetBuffer.comp
    Video/shaders/ComputeVertexBuffer.comp
    Video/shaders/ComputeVertexBuffer.vert
    Video/shaders/FullscreenTriangle.vert
    Video/shaders/DrawMipmappedTriangle.frag
    Video/shaders/DrawPushTriangles.vert
    Video/shaders/DrawStorageTriangle.vert
    Video/shaders/DrawTexturedTriangle.vert
    Video/shaders/DrawTexturedTriangle.frag
    Video/shaders/DrawTriangle.vert
    Video/shaders/DrawTriangle.frag
    Video/shaders/DrawTriangles.vert
    Video/shaders/DrawVertexTriangle.vert
    Video/shaders/InvertColors.frag
    Video/shaders/MultipleFrames.vert
    Video/shaders/MultipleFrames.frag
    Video/shaders/StorageBuffer.frag
)

# Setup images.
set(IMAGES
    Video/textures/Test.png
    Video/references/ComputeSetBuffer.png
    Video/references/DepthPrePass.png
    Video/references/DrawLines.png
    Video/references/DrawMipmappedTriangle.png
    Video/references/DrawPushTriangles.png
    Video/references/DrawTexturedTriangle.png
    Video/references/DrawTriangle.png
    Video/references/DrawTriangles.png
    Video/references/DrawQuad.png
    Video/references/InvertColors.png
)

# Process shaders.
foreach(item ${SHADERS})
    ProcessShader(SHADER_HEADER SHADER_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR} ON)
endforeach()

# Process images.
foreach(item ${IMAGES})
    ProcessWithHeaderize(IMAGES_HEADER IMAGES_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set_property(SOURCE ${SRCS} APPEND PROPERTY OBJECT_DEPENDS ${SHADER_HEADER} ${IMAGES_HEADER})

if(OpenGLRenderer)
    add_definitions(-DOPENGL_SUPPORT)
endif()
if(VulkanRenderer)
    add_definitions(-DVULKAN_SUPPORT)
endif()

create_directory_groups(${SRCS} ${HEADERS})

include_directories(../src)
add_executable(Tests ${SRCS} ${HEADERS} ${SHADER_SOURCE} ${IMAGES_SOURCE})
target_link_libraries(Tests Video)

set_property(TARGET Tests PROPERTY CXX_STANDARD 11)
set_property(TARGET Tests PROPERTY CXX_STANDARD_REQUIRED ON)
