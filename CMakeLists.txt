cmake_minimum_required(VERSION 3.5)

project(solver-rc LANGUAGES CXX C)
include(FetchContent)

# setting required options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FETCHCONTENT_QUIET OFF)
set(OpenGL_GL_PREFERENCE "GLVND")

option(SOLVER_BUILD_TESTS "Build tests" FALSE)

# glfw
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_PROGRESS TRUE
)

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_PROGRESS TRUE
)

# glad installation from the tutorial
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad
    GIT_TAG v2.0.6
    GIT_PROGRESS TRUE
)
if(NOT glad_POPULATED)
    message("Fetching glad")
    FetchContent_MakeAvailable(glad)

    add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
    glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.6)
endif()

# glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(imgui glfw glm)

# assuming everybody has OpenGL
find_package(OpenGL REQUIRED)

# installation required
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)

if(${SOLVER_BUILD_TESTS})
    message(NOTICE "Building tests")
    enable_testing()

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(doctest)
    add_subdirectory(tests)
endif()
