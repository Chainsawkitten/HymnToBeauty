include_directories(.)

add_subdirectory(Utility)
add_subdirectory(Video)
add_subdirectory(Engine)
if(NOT ANDROID)
    add_subdirectory(Editor)
endif()
add_subdirectory(Game)
