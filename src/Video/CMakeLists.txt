set(SRCS
    DebugDrawing.cpp
    Renderer.cpp
    Culling/AxisAlignedBoundingBox.cpp
    Culling/Frustum.cpp
    Culling/Sphere.cpp
    Geometry/Geometry2D.cpp
    Geometry/Geometry3D.cpp
    Geometry/Rectangle.cpp
    Geometry/VertexType/StaticVertex.cpp
    Lighting/Isocahedron.cpp
    Lighting/ZBinning.cpp
    PostProcessing/PostProcessing.cpp
    RenderProgram/RenderProgram.cpp
    RenderProgram/SpriteRenderProgram.cpp
    RenderProgram/StaticRenderProgram.cpp
    Texture/Texture2D.cpp
    LowLevelRenderer/Interface/BufferAllocator.cpp
    LowLevelRenderer/Interface/RenderPassAllocator.cpp
    LowLevelRenderer/Interface/RenderTargetAllocator.cpp
    LowLevelRenderer/Interface/Texture.cpp
)

set(HEADERS
    DebugDrawing.hpp
    Renderer.hpp
    RenderScene.hpp
    Culling/AxisAlignedBoundingBox.hpp
    Culling/Frustum.hpp
    Culling/Sphere.hpp
    Geometry/Geometry2D.hpp
    Geometry/Geometry3D.hpp
    Geometry/Rectangle.hpp
    Geometry/VertexType/StaticVertex.hpp
    Lighting/Isocahedron.hpp
    Lighting/Light.hpp
    Lighting/ZBinning.hpp
    PostProcessing/PostProcessing.hpp
    RenderProgram/RenderProgram.hpp
    RenderProgram/SpriteRenderProgram.hpp
    RenderProgram/StaticRenderProgram.hpp
    Texture/Texture2D.hpp
    LowLevelRenderer/Interface/LowLevelRenderer.hpp
    LowLevelRenderer/Interface/Buffer.hpp
    LowLevelRenderer/Interface/BufferAllocator.hpp
    LowLevelRenderer/Interface/CommandBuffer.hpp
    LowLevelRenderer/Interface/ComputePipeline.hpp
    LowLevelRenderer/Interface/GeometryBinding.hpp
    LowLevelRenderer/Interface/GraphicsPipeline.hpp
    LowLevelRenderer/Interface/RenderPass.hpp
    LowLevelRenderer/Interface/RenderPassAllocator.hpp
    LowLevelRenderer/Interface/RenderTargetAllocator.hpp
    LowLevelRenderer/Interface/Sampler.hpp
    LowLevelRenderer/Interface/Shader.hpp
    LowLevelRenderer/Interface/ShaderProgram.hpp
    LowLevelRenderer/Interface/Texture.hpp
    LowLevelRenderer/Interface/Types.hpp
    LowLevelRenderer/Interface/VertexDescription.hpp
)

add_library(Video STATIC)

# Graphics API libraries to link to.
set(API_LIBRARIES)

if(OpenGLRenderer)
    target_link_libraries(Video glad)
    target_compile_definitions(Video PUBLIC OPENGL_SUPPORT)

    set(SRCS
        ${SRCS}
        LowLevelRenderer/OpenGL/OpenGLRenderer.cpp
        LowLevelRenderer/OpenGL/OpenGLBuffer.cpp
        LowLevelRenderer/OpenGL/OpenGLBufferAllocator.cpp
        LowLevelRenderer/OpenGL/OpenGLCommandBuffer.cpp
        LowLevelRenderer/OpenGL/OpenGLComputePipeline.cpp
        LowLevelRenderer/OpenGL/OpenGLGeometryBinding.cpp
        LowLevelRenderer/OpenGL/OpenGLGraphicsPipeline.cpp
        LowLevelRenderer/OpenGL/OpenGLRenderPass.cpp
        LowLevelRenderer/OpenGL/OpenGLRenderPassAllocator.cpp
        LowLevelRenderer/OpenGL/OpenGLRenderTargetAllocator.cpp
        LowLevelRenderer/OpenGL/OpenGLSampler.cpp
        LowLevelRenderer/OpenGL/OpenGLShader.cpp
        LowLevelRenderer/OpenGL/OpenGLShaderProgram.cpp
        LowLevelRenderer/OpenGL/OpenGLTexture.cpp
        LowLevelRenderer/OpenGL/OpenGLVertexDescription.cpp
    )

    set(HEADERS
        ${HEADERS}
        LowLevelRenderer/OpenGL/OpenGLRenderer.hpp
        LowLevelRenderer/OpenGL/OpenGLBuffer.hpp
        LowLevelRenderer/OpenGL/OpenGLBufferAllocator.hpp
        LowLevelRenderer/OpenGL/OpenGLCommandBuffer.hpp
        LowLevelRenderer/OpenGL/OpenGLComputePipeline.hpp
        LowLevelRenderer/OpenGL/OpenGLGeometryBinding.hpp
        LowLevelRenderer/OpenGL/OpenGLGraphicsPipeline.hpp
        LowLevelRenderer/OpenGL/OpenGLRenderPass.hpp
        LowLevelRenderer/OpenGL/OpenGLRenderPassAllocator.hpp
        LowLevelRenderer/OpenGL/OpenGLRenderTargetAllocator.hpp
        LowLevelRenderer/OpenGL/OpenGLSampler.hpp
        LowLevelRenderer/OpenGL/OpenGLShader.hpp
        LowLevelRenderer/OpenGL/OpenGLShaderProgram.hpp
        LowLevelRenderer/OpenGL/OpenGLTexture.hpp
        LowLevelRenderer/OpenGL/OpenGLVertexDescription.hpp
    )
endif()

if(VulkanRenderer)
    find_package(VULKAN REQUIRED)

    target_link_libraries(Video Vulkan::Vulkan)
    target_compile_definitions(Video PUBLIC VULKAN_SUPPORT)

    set(SRCS
        ${SRCS}
        LowLevelRenderer/Vulkan/VulkanRenderer.cpp
        LowLevelRenderer/Vulkan/VulkanBuffer.cpp
        LowLevelRenderer/Vulkan/VulkanBufferAllocator.cpp
        LowLevelRenderer/Vulkan/VulkanCommandBuffer.cpp
        LowLevelRenderer/Vulkan/VulkanComputePipeline.cpp
        LowLevelRenderer/Vulkan/VulkanGeometryBinding.cpp
        LowLevelRenderer/Vulkan/VulkanGraphicsPipeline.cpp
        LowLevelRenderer/Vulkan/VulkanRenderPass.cpp
        LowLevelRenderer/Vulkan/VulkanRenderPassAllocator.cpp
        LowLevelRenderer/Vulkan/VulkanRenderTargetAllocator.cpp
        LowLevelRenderer/Vulkan/VulkanSampler.cpp
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
        LowLevelRenderer/Vulkan/VulkanBufferAllocator.hpp
        LowLevelRenderer/Vulkan/VulkanCommandBuffer.hpp
        LowLevelRenderer/Vulkan/VulkanComputePipeline.hpp
        LowLevelRenderer/Vulkan/VulkanGeometryBinding.hpp
        LowLevelRenderer/Vulkan/VulkanGraphicsPipeline.hpp
        LowLevelRenderer/Vulkan/VulkanRenderPass.hpp
        LowLevelRenderer/Vulkan/VulkanRenderPassAllocator.hpp
        LowLevelRenderer/Vulkan/VulkanRenderTargetAllocator.hpp
        LowLevelRenderer/Vulkan/VulkanSampler.hpp
        LowLevelRenderer/Vulkan/VulkanShader.hpp
        LowLevelRenderer/Vulkan/VulkanShaderProgram.hpp
        LowLevelRenderer/Vulkan/VulkanTexture.hpp
        LowLevelRenderer/Vulkan/VulkanUtility.hpp
        LowLevelRenderer/Vulkan/VulkanVertexDescription.hpp
    )
endif()

if (WebGPURenderer)
    if (WEBGPU_BACKEND_DAWN)
        target_link_libraries(Video dawn_native dawn_proc)
        target_compile_definitions(Video PUBLIC WEBGPU_BACKEND_DAWN)
    elseif(WEBGPU_BACKEND_WGPU)
        target_link_libraries(Video ${WGPU_LIBRARY} wgpu_headers ${WGPU_OS_LIBRARIES})
        target_compile_definitions(Video PUBLIC WEBGPU_BACKEND_WGPU)
        
        # Skip local lights to work around no atomics in wgpu.
        # Remove this once wgpu has atomics.
        target_compile_definitions(Video PRIVATE SKIP_LOCAL_LIGHTS)
    endif()
    
    target_compile_definitions(Video PUBLIC WEBGPU_SUPPORT)
    
    set(SRCS
        ${SRCS}
        LowLevelRenderer/WebGPU/WebGPURenderer.cpp
        LowLevelRenderer/WebGPU/WebGPUBuffer.cpp
        LowLevelRenderer/WebGPU/WebGPUCommandBuffer.cpp
        LowLevelRenderer/WebGPU/WebGPUComputePipeline.cpp
        LowLevelRenderer/WebGPU/WebGPUGeometryBinding.cpp
        LowLevelRenderer/WebGPU/WebGPUGraphicsPipeline.cpp
        LowLevelRenderer/WebGPU/WebGPURenderTargetAllocator.cpp
        LowLevelRenderer/WebGPU/WebGPUSampler.cpp
        LowLevelRenderer/WebGPU/WebGPUShader.cpp
        LowLevelRenderer/WebGPU/WebGPUShaderProgram.cpp
        LowLevelRenderer/WebGPU/WebGPUTexture.cpp
        LowLevelRenderer/WebGPU/WebGPUVertexDescription.cpp
    )
    
    set(HEADERS
        ${HEADERS}
        LowLevelRenderer/WebGPU/WebGPU.hpp
        LowLevelRenderer/WebGPU/WebGPURenderer.hpp
        LowLevelRenderer/WebGPU/WebGPUBuffer.hpp
        LowLevelRenderer/WebGPU/WebGPUCommandBuffer.hpp
        LowLevelRenderer/WebGPU/WebGPUComputePipeline.hpp
        LowLevelRenderer/WebGPU/WebGPUGeometryBinding.hpp
        LowLevelRenderer/WebGPU/WebGPUGraphicsPipeline.hpp
        LowLevelRenderer/WebGPU/WebGPURenderTargetAllocator.hpp
        LowLevelRenderer/WebGPU/WebGPUSampler.hpp
        LowLevelRenderer/WebGPU/WebGPUShader.hpp
        LowLevelRenderer/WebGPU/WebGPUShaderProgram.hpp
        LowLevelRenderer/WebGPU/WebGPUTexture.hpp
        LowLevelRenderer/WebGPU/WebGPUVertexDescription.hpp
    )
endif()

# Setup shaders
set(SHADER_INCLUDES
    shaders/PostProcessing/Dither.glsl
)

set(SHADERS
    shaders/DebugDrawing.vert
    shaders/DebugDrawingPoint.vert
    shaders/DebugDrawing.frag
    shaders/Default3D.vert
    shaders/Default3D.frag
    shaders/DepthPrePass.frag
    shaders/DepthPrePassStatic.vert
    shaders/EditorEntity.vert
    shaders/EditorEntity.frag
    shaders/LightBinning.comp
    shaders/LightTiling.vert
    shaders/LightTiling.frag
    shaders/SampleTexture.frag
    shaders/Sprite.vert
    shaders/Sprite.frag
    shaders/PostProcessing/BloomBlur.frag
    shaders/PostProcessing/BloomDownscale.frag
    shaders/PostProcessing/BloomThreshold.frag
    shaders/PostProcessing/BloomUpscale.frag
    shaders/PostProcessing/FXAA.frag
    shaders/PostProcessing/PostProcessing.vert
    shaders/PostProcessing/UberPostProcessing.frag
)

# Process shaders.
foreach(item ${SHADERS})
    ProcessShader(SHADER_HEADER SHADER_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR} ${SHADER_INCLUDES})
endforeach()

create_directory_groups(${SRCS} ${HEADERS})

target_sources(Video PRIVATE ${SRCS} ${HEADERS} ${SHADER_SOURCE} ${SHADER_HEADER})
target_link_libraries(Video Utility glm stb ShaderSource)

set_property(TARGET Video PROPERTY CXX_STANDARD 17)
set_property(TARGET Video PROPERTY CXX_STANDARD_REQUIRED ON)
