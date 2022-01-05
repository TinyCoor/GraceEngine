

set(HEADERS src/core/logger.h src/core/asserts.h)
set(SRCS src/core/logger.c)

add_library(engine ${SRCS} ${HEADERS})
