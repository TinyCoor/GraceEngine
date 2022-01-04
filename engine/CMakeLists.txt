

set(HEADERS src/test.h)
set(SRCS src/test.c)

add_library(engine ${SRCS} ${HEADERS})

set_target_properties(engine PROPERTIES INCLUDE_DIRS ${HEADERS} )