set(SRCS
    DebugDrawing.cpp
    Renderer.cpp
    Culling/AxisAlignedBoundingBox.cpp
    Culling/Frustum.cpp
    Geometry/Geometry2D.cpp
    Geometry/Geometry3D.cpp
    Geometry/Rectangle.cpp
    Geometry/VertexType/StaticVertex.cpp
    PostProcessing/PostProcessing.cpp
    RenderProgram/RenderProgram.cpp
    RenderProgram/StaticRenderProgram.cpp
    Texture/Texture2D.cpp
)

set(HEADERS
    DebugDrawing.hpp
    Renderer.hpp
    Culling/AxisAlignedBoundingBox.hpp
    Culling/Frustum.hpp
    Geometry/Geometry2D.hpp
    Geometry/Geometry3D.hpp
    Geometry/Rectangle.hpp
    Geometry/VertexType/StaticVertex.hpp
    Lighting/Light.hpp
    PostProcessing/PostProcessing.hpp
    RenderProgram/RenderProgram.hpp
    RenderProgram/StaticRenderProgram.hpp
    Texture/Texture2D.hpp
    LowLevelRenderer/Interface/LowLevelRenderer.hpp
    LowLevelRenderer/Interface/Buffer.hpp
    LowLevelRenderer/Interface/CommandBuffer.hpp
    LowLevelRenderer/Interface/ComputePipeline.hpp
    LowLevelRenderer/Interface/GeometryBinding.hpp
    LowLevelRenderer/Interface/GraphicsPipeline.hpp
    LowLevelRenderer/Interface/RenderPass.hpp
    LowLevelRenderer/Interface/Shader.hpp
    LowLevelRenderer/Interface/ShaderProgram.hpp
    LowLevelRenderer/Interface/Texture.hpp
    LowLevelRenderer/Interface/Types.hpp
    LowLevelRenderer/Interface/VertexDescription.hpp
)

# Graphics API libraries to link to.
set(API_LIBRARIES)

if(OpenGLRenderer)
    set(API_LIBRARIES
        ${API_LIBRARIES}
        glad
    )

    add_definitions(-DOPENGL_SUPPORT)

    set(SRCS
        ${SRCS}
        LowLevelRenderer/OpenGL/OpenGLRenderer.cpp
        LowLevelRenderer/OpenGL/OpenGLBuffer.cpp
        LowLevelRenderer/OpenGL/OpenGLCommandBuffer.cpp
        LowLevelRenderer/OpenGL/OpenGLComputePipeline.cpp
        LowLevelRenderer/OpenGL/OpenGLGeometryBinding.cpp
        LowLevelRenderer/OpenGL/OpenGLGraphicsPipeline.cpp
        LowLevelRenderer/OpenGL/OpenGLRenderPass.cpp
        LowLevelRenderer/OpenGL/OpenGLShader.cpp
        LowLevelRenderer/OpenGL/OpenGLShaderProgram.cpp
        LowLevelRenderer/OpenGL/OpenGLTexture.cpp
        LowLevelRenderer/OpenGL/OpenGLVertexDescription.cpp
    )

    set(HEADERS
        ${HEADERS}
        LowLevelRenderer/OpenGL/OpenGLRenderer.hpp
        LowLevelRenderer/OpenGL/OpenGLBuffer.hpp
        LowLevelRenderer/OpenGL/OpenGLCommandBuffer.hpp
        LowLevelRenderer/OpenGL/OpenGLComputePipeline.hpp
        LowLevelRenderer/OpenGL/OpenGLGeometryBinding.hpp
        LowLevelRenderer/OpenGL/OpenGLGraphicsPipeline.hpp
        LowLevelRenderer/OpenGL/OpenGLRenderPass.hpp
        LowLevelRenderer/OpenGL/OpenGLShader.hpp
        LowLevelRenderer/OpenGL/OpenGLShaderProgram.hpp
        LowLevelRenderer/OpenGL/OpenGLTexture.hpp
        LowLevelRenderer/OpenGL/OpenGLVertexDescription.hpp
    )
endif()

if(VulkanRenderer)
    find_package(VULKAN REQUIRED)
    set(API_LIBRARIES
        ${API_LIBRARIES}
        Vulkan::Vulkan
    )

    add_definitions(-DVULKAN_SUPPORT)

    set(SRCS
        ${SRCS}
        LowLevelRenderer/Vulkan/VulkanRenderer.cpp
        LowLevelRenderer/Vulkan/VulkanBuffer.cpp
        LowLevelRenderer/Vulkan/VulkanCommandBuffer.cpp
        LowLevelRenderer/Vulkan/VulkanComputePipeline.cpp
        LowLevelRenderer/Vulkan/VulkanGeometryBinding.cpp
        LowLevelRenderer/Vulkan/VulkanGraphicsPipeline.cpp
        LowLevelRenderer/Vulkan/VulkanRenderPass.cpp
        LowLevelRenderer/Vulkan/VulkanShader.cpp
        LowLevelRenderer/Vulkan/VulkanShaderProgram.cpp
        LowLevelRenderer/Vulkan/VulkanTexture.cpp
        LowLevelRenderer/Vulkan/VulkanUtility.cpp
        LowLevelRenderer/Vulkan/VulkanVertexDescription.cpp
    )

    set(HEADERS
        ${HEADERS}
        LowLevelRenderer/Vulkan/VulkanRenderer.hpp
        LowLevelRenderer/Vulkan/VulkanBuffer.hpp
        LowLevelRenderer/Vulkan/VulkanCommandBuffer.hpp
        LowLevelRenderer/Vulkan/VulkanComputePipeline.hpp
        LowLevelRenderer/Vulkan/VulkanGeometryBinding.hpp
        LowLevelRenderer/Vulkan/VulkanGraphicsPipeline.hpp
        LowLevelRenderer/Vulkan/VulkanRenderPass.hpp
        LowLevelRenderer/Vulkan/VulkanShader.hpp
        LowLevelRenderer/Vulkan/VulkanShaderProgram.hpp
        LowLevelRenderer/Vulkan/VulkanTexture.hpp
        LowLevelRenderer/Vulkan/VulkanUtility.hpp
        LowLevelRenderer/Vulkan/VulkanVertexDescription.hpp
    )
endif()

# Setup shaders
set(SHADERS
    shaders/DebugDrawing.vert
    shaders/DebugDrawing.frag
    shaders/Default3D.vert
    shaders/Default3D.frag
    shaders/DepthPrePass.frag
    shaders/DepthPrePassStatic.vert
    shaders/EditorEntity.vert
    shaders/EditorEntity.frag
    shaders/FXAA.frag
    shaders/PostProcessing.vert
    shaders/SampleTexture.frag
    shaders/UberPostProcessing.frag
)

# Process shaders.
foreach(item ${SHADERS})
    ProcessShader(SHADER_HEADER SHADER_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set_property(SOURCE ${SRCS} APPEND PROPERTY OBJECT_DEPENDS ${SHADER_HEADER})

create_directory_groups(${SRCS} ${HEADERS})

add_library(Video STATIC ${SRCS} ${HEADERS} ${SHADER_SOURCE})
target_link_libraries(Video Utility ${API_LIBRARIES} glfw ${GLFW_LIBRARIES} glm jsoncpp_lib_static stb ShaderSource)

set_property(TARGET Video PROPERTY CXX_STANDARD 11)
set_property(TARGET Video PROPERTY CXX_STANDARD_REQUIRED ON)
