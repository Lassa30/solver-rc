include_directories(${doctest_SOURCE_DIR}/doctest)
add_executable(test_cube test_cube.cpp)
target_link_libraries(test_cube cube doctest)
add_test(NAME cube COMMAND test_cube)
