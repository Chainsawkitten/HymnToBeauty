set(SRCS
        Log.cpp
        Time.cpp
        Window.cpp
        Profiling/Event.cpp
        Profiling/Profiling.cpp
        Profiling/Thread.cpp
        Profiling/Timeline.cpp
    )

set(HEADERS
        Queue.hpp
        LockBox.hpp
        Log.hpp
        Time.hpp
        Window.hpp
        Profiling/Event.hpp
        Profiling/Profiling.hpp
        Profiling/Thread.hpp
        Profiling/Timeline.hpp
    )

if (ANDROID)
    find_package(game-activity REQUIRED CONFIG)
    get_target_property(game-activity-include
            game-activity::game-activity
            INTERFACE_INCLUDE_DIRECTORIES)
endif()

create_directory_groups(${SRCS} ${HEADERS})

add_library(Utility STATIC ${SRCS} ${HEADERS})
target_link_libraries(Utility glm jsoncpp_static)
if (ANDROID)
    target_link_libraries(Utility game-activity::game-activity log android)
else()
    target_link_libraries(Utility glfw ${GLFW_LIBRARIES})
endif()

set_property(TARGET Utility PROPERTY CXX_STANDARD 11)
set_property(TARGET Utility PROPERTY CXX_STANDARD_REQUIRED ON)
