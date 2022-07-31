if (ANDROID)
    # Build as a library to include in an Android app.
    add_library(hymntobeauty
            SHARED
            AndroidMain.cpp
            GameActivitySources.cpp
            )

    target_link_libraries(hymntobeauty Engine)
else()
    # Build as an executable.
    set(SRCS
            main.cpp
            Util/GameSettings.cpp
        )

    set(HEADERS
            Util/GameSettings.hpp
        )

    create_directory_groups(${SRCS} ${HEADERS})

    add_executable(Game ${SRCS} ${HEADERS})
    target_link_libraries(Game Engine)

    set_property(TARGET Game PROPERTY CXX_STANDARD 11)
    set_property(TARGET Game PROPERTY CXX_STANDARD_REQUIRED ON)
endif()