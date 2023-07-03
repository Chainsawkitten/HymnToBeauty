set(SRCS
        Framework/Runner.cpp
        Framework/SynchronizationTest.cpp
        Tests/ExampleTest.cpp
    )

set(HEADERS
        Framework/Runner.hpp
        Framework/SynchronizationTest.hpp
        Tests/ExampleTest.hpp
    )
        
create_directory_groups(${SRCS} ${HEADERS})

if (ANDROID)
    # Build as a library to include in an Android app.
    add_library(synchronizationtests
            SHARED
            AndroidMain.cpp
            GameActivitySources.cpp
            ${SRCS}
            ${HEADERS}
            )

    target_link_libraries(synchronizationtests Video)
else()
    # Build as an executable
    add_executable(SynchronizationTests main.cpp ${SRCS} ${HEADERS})
    target_link_libraries(SynchronizationTests Video)

    set_property(TARGET SynchronizationTests PROPERTY CXX_STANDARD 11)
    set_property(TARGET SynchronizationTests PROPERTY CXX_STANDARD_REQUIRED ON)
endif()