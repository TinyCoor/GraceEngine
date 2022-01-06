

set(HEADERS
        src/core/logger.h
        src/core/asserts.h

        src/platform/platform.h
        src/platform/platform_win32.c
        )

set(SRCS src/core/logger.c)

add_library(engine ${SRCS} ${HEADERS})
