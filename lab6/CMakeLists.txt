add_executable(lab6.elf main.c bhTester.c buttonHandler.c globals.c flashSequence.c vsTester.c fsTester.c simonControl.c simonDisplay.c verifySequence.c )
target_link_libraries(lab6.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab6.elf PROPERTIES LINKER_LANGUAGE CXX)

