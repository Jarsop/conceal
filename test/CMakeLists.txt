add_executable(conceal_test test_main.cpp)
target_link_libraries(conceal_test PRIVATE conceal)
add_test(NAME conceal_test COMMAND conceal_test)