add_executable(snake.elf main.c)
target_link_libraries(snake.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(snake.elf PROPERTIES LINKER_LANGUAGE CXX)