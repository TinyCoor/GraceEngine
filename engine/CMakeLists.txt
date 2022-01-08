

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
        src/core/input.h
        src/core/event.h
        src/core/application.c
        src/core/kmemory.c
        src/core/event.c
        src/core/input.c
        src/platform/platform.h
        src/container/darray.h
        src/container/darray.c
        ${platform_src}
        src/platform/platform_linux.c
        )

set(SRCS src/core/logger.c)

add_library(engine ${SRCS} ${HEADERS})

target_include_directories(engine PUBLIC src)
