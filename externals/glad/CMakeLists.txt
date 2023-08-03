add_library(glad STATIC
    src/gl.c
    src/vulkan.c
)

target_include_directories(glad PUBLIC "include/")

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    target_link_libraries(glad PRIVATE dl)
endif()
