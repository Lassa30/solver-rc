# Create imgui static library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
)

add_library(utils INTERFACE utils.hpp)
add_executable(solver-rc
    main.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(solver-rc PRIVATE
    ${glad_SOURCE_DIR}
    ${glm_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(solver-rc PRIVATE
    utils
    imgui
    glad
    glfw
    OpenGL::GL
    glm
    ${OpenCV_LIBS}
)
