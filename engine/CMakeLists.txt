

set(platform_src )

if (CMAKE_SYSTEM_NAME MATCHES "Window")
    list(APPEND
            platform_src src/platform/platform_win32.c)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    list(APPEND platform_src
            src/platform/platform_linux.c)
endif ()

set(HEADERS
        src/core/logger.h
        src/core/asserts.h
        src/core/application.c
        src/platform/platform.h

        ${platform_src}
        )

set(SRCS src/core/logger.c)

add_library(engine ${SRCS} ${HEADERS})
