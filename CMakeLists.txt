cmake_minimum_required(VERSION 3.5)

project(solver-rc LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# glfw
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_PROGRESS   TRUE
)

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_PROGRESS   TRUE
)

include(FetchContent)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad
    GIT_TAG        v2.0.6
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(glew)
if(NOT glad_POPULATED)
    message("Fetching glad")
    FetchContent_MakeAvailable(glad)

    add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
    glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.6 EXTENSIONS GL_ARB_bindless_texture GL_EXT_texture_compression_s3tc)
endif()

FetchContent_MakeAvailable(imgui glfw)

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

find_package(OpenCV REQUIRED)

add_subdirectory(solver-rc)
