enable_testing()

add_executable(test_main src/entry.c src/game.h src/game.c)
target_include_directories(test_main PRIVATE ../engine/src)
target_link_libraries(test_main PRIVATE engine)
add_test(NAMES Test COMMAND $<TARGET_FILE:test_main>)