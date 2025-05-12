include_directories(${doctest_SOURCE_DIR}/doctest)

add_executable(test_cube test_cube.cpp)
add_executable(test_solver test_solver.cpp)

target_link_libraries(test_cube cube doctest)
target_link_libraries(test_solver cube doctest)

add_test(NAME cube COMMAND test_cube)
add_test(NAME solver COMMAND test_solver)
