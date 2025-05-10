#include <cube/cube.hpp>
#include <solvers/solvers.hpp>
#include <utils.hpp>

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

/*UTILS*/
static void glfw_error_callback(int error, const char *description) {
  std::cout << (stderr, "GLFW Error %d: %s\n", error, description);
}

static void printShaderRelatedError(int &infoLength, GLuint &shaderProgram) {
  std::vector<char> ProgramErrorMessage(infoLength + 1);
  glGetProgramInfoLog(shaderProgram, infoLength, nullptr,
                      &ProgramErrorMessage[0]);
  printf("%s\n", &ProgramErrorMessage[0]);
}

GLuint compileShaders() {
  GLint compileStatus;
  int infoLength;

  // reading vertex shader
  std::string vertexShaderSource =
      readShader("../src/shaders/vertexShader.vert");
  const char *vertexShaderSource_cstr = vertexShaderSource.c_str();

  // reading fragment shader
  std::string fragmentShaderSource =
      readShader("../src/shaders/fragmentShader.frag");
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

void render() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
}

/*Variables*/
static float x = 0.2f;

static float Vertices[3][7] = {
    {-x, x, 0, 1, 0, 0, 1}, {x, x, 0, 1, 0, 0, 1}, {x, -x, 0, 1, 0, 0, 1}};

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
  });
  // clang-format on

  // window creation
  glfwMakeContextCurrent(window);
  if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  // turn on vsync
  glfwSwapInterval(1);

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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void *)(3 * sizeof(float)));
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
[Experiments]
1) Compare performance:
- Compute ModelViewProjection matrices each frame on CPU and the final result on
GPU <i.e. MVP * vec4(...)>
- Give everything to GPU and compute it there < i.e. projection * view * model *
vec4(...) >
*/