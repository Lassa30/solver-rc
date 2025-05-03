#include "utils.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

// clang-format off
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <chrono>
#include <iostream>
#include <random>

/*
GL RELATED UTILS
*/
static void glfw_error_callback(int error, const char *description) {
  std::cout << (stderr, "GLFW Error %d: %s\n", error, description);
}

static void printShaderRelatedError(int &infoLength, GLuint &shaderProgram) {
  std::vector<char> ProgramErrorMessage(infoLength + 1);
  glGetProgramInfoLog(shaderProgram, infoLength, nullptr,
                      &ProgramErrorMessage[0]);
  printf("%s\n", &ProgramErrorMessage[0]);
}

/*
***************************************************
*  cube                                           *
***************************************************
*/

// clang-format off
enum class ColorID { Color_1, Color_2, Color_3, Color_4, Color_5, Color_6, Color_None };
// clang-format on

// struct Colors {
//   glm::vec4 FrontColor{1.0f, 1.0f, 1.0f, 1.0f};  // White
//   glm::vec4 BackColor{1.0f, 1.0f, 0.0f, 1.0f};   // Yellow
//   glm::vec4 RightColor{1.0f, 0.0f, 0.0f, 1.0f};  // Red
//   glm::vec4 LeftColor{1.0f, 0.5f, 0.0f, 1.0f};   // Orange
//   glm::vec4 TopColor{0.0f, 0.0f, 1.0f, 1.0f};    // Blue
//   glm::vec4 BottomColor{0.0f, 1.0f, 0.0f, 1.0f}; // Green
// };

struct Cubie {
  std::array<ColorID, 3> colors;
  glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

  Cubie() = default;
};

class RubiksCube {
  std::vector<Cubie> cube;
};

class CubeRenderer;
class CubieRenderer;

static float x = 0.2f;

static float Vertices[3][7] = {
    {-x, x, 0, 1, 0, 0, 1}, {x, x, 0, 1, 0, 0, 1}, {x, -x, 0, 1, 0, 0, 1}};

void render() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
}

GLuint compileShaders() {
  GLint compileStatus;
  int infoLength;

  // reading vertex shader
  std::string vertexShaderSource =
      readShader("../solver-rc/shaders/vertexShader.vert");
  const char *vertexShaderSource_cstr = vertexShaderSource.c_str();

  // reading fragment shader
  std::string fragmentShaderSource =
      readShader("../solver-rc/shaders/fragmentShader.frag");
  const char *fragmentShaderSource_cstr = fragmentShaderSource.c_str();

  // compiling vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource_cstr, nullptr);
  glCompileShader(vertexShader);
  // checking vertex shader
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
  glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLength);
  if (infoLength > 0) {
    printShaderRelatedError(infoLength, vertexShader);
  }

  // compiling fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource_cstr, nullptr);
  glCompileShader(fragmentShader);
  // checking fragment shader
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);
  glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLength);
  if (infoLength > 0) {
    printShaderRelatedError(infoLength, fragmentShader);
  }

  // linking shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // check the program
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &compileStatus);
  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLength);
  if (infoLength > 0) {
    printShaderRelatedError(infoLength, shaderProgram);
  }

  // delete shaders: they are compiled already
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

int main() {
  // clang-format off
  if (!glfwInit()) {
    return 1;
  }
  int xSideSize = 800, ySideSize = 600;
  GLFWwindow *window = glfwCreateWindow(xSideSize, ySideSize, "Rubik's Cube Solver", nullptr, nullptr);

  if (!window) {
    return 1;
  }

  // callbacks
  glfwSetErrorCallback(glfw_error_callback);
  glfwSetFramebufferSizeCallback(
    window, 
    [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }
  );
  glfwSetKeyCallback(
    window, 
    [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      int width, height;
      glfwGetWindowSize(window, &width, &height);

      switch (key) {
      case GLFW_KEY_UP:
        height -= 25;
        break;
      case GLFW_KEY_DOWN:
        height += 25;
        break;
      case GLFW_KEY_LEFT:
        width -= 25;
        break;
      case GLFW_KEY_RIGHT:
        width += 25;
        break;
      default:
        return;
      }

      // Ensure minimum size
      width = std::max(width, 100);
      height = std::max(height, 100);
      
      glfwSetWindowSize(window, width, height);
    }
  }); // clang-format on

  // window creation
  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glfwSwapInterval(1); // vsync on

  // compiling shaders
  auto shaderProgram = compileShaders();
  glUseProgram(shaderProgram);

  // init buffer
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);

  // size related
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // clang-format off
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
  // clang-format on
  glEnableVertexAttribArray(1);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

/*
[DEFINITIONS]
  glsl           : language to write shaders
  vertex shader  : sets the position and the color for the each vertex
  fragment shader: takes input from the vertex shader and then do the stuff

[NOTES]
1)
  inside fragment shader:
  FragColor = vec4(...) is actually a cycle, check pseudo code:

  for frag_idx in range(len(all_fragments)):
    FragColor[frag_idx] = vec4(...)\

2)
why do we need a first line, we don't use it at all...
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
  glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLength);

3)
I think this code is about resize handling, but what it really does?
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

4)
  the right order:
  line n  : create context
  line n+1:     setup glad

[Experiments]
1) Compare performance
- Compute ModelViewProjection matrices on CPU and the final compileStatus on GPU
- Give everything to GPU and compute it
*/