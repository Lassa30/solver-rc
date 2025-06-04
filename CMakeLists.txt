cmake_minimum_required(VERSION 3.5)

project(solver-rc LANGUAGES CXX C)
include(FetchContent)

# setting required options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FETCHCONTENT_QUIET ON)
set(CPR_ENABLE_SSL OFF)

option(SOLVER_DEBUG "Build with debug information" OFF)
if(${SOLVER_DEBUG})
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()

# assuming everybody has OpenGL
find_package(OpenGL REQUIRED)

# installation required
find_package(GLUT REQUIRED)

# troll
add_library(utils INTERFACE utils.hpp)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)
target_link_libraries(utils INTERFACE cpr::cpr)

# solver-rc main
add_executable(solver-rc
    main.cpp
)

target_include_directories(solver-rc PRIVATE
    ${GLUT_INCLUDE_DIRS}
)
target_link_libraries(solver-rc PRIVATE
    utils
    OpenGL::GL
    ${GLUT_LIBRARIES}
)
