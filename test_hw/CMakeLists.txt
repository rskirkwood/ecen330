add_executable(test_hw.elf test_hw.c)
target_link_libraries(test_hw.elf ${330_LIBS})
set_target_properties(test_hw.elf PROPERTIES LINKER_LANGUAGE CXX)