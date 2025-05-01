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
  /*colors of a cubie:
  center - 1 color
  edge - 2 colors
  corner - 3 colors
  */
  std::array<ColorID, 3> colors;
  glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

  Cubie() = default;
};

class RubiksCube {
  std::vector<Cubie> cube;
};

class CubeRenderer;
class CubieRenderer;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distrib(0, 1);

static void glfw_error_callback(int error, const char *description) {
  std::cout << (stderr, "GLFW Error %d: %s\n", error, description);
}

float r1 = 0;
float g1 = 1;
float b1 = 0;

float r2 = 1.0;
float g2 = 0;
float b2 = 1.0;
float x = 0.1f;

float Vertices[3][7] = {{-x, x, 0, r1, g1, b1, 1},
                        {x, x, 0, 0, 0, 0, 1},
                        {x, -x, 0, r2, g2, b2, 1}};

void do_render(GLuint VAO, GLuint VBO) {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

GLuint compileShaders() {
  /*glsl language to write shaders
  vertex shader: sets the position and the color for the each vertex
  */

  /*scale-rotate-translate vertexShader
  this code should better be outside of the shader
  shader code better be just: MVP * vect
  */
  const char *vertexShaderSource = R"glsl(
    #version 460 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    out vec4 vertexColor;
    
    uniform float time;
    float k = abs(sin(time)) + 0.5;
    mat4 scale = mat4(
      vec4(k, 0, 0, 0),
      vec4(0, k, 0, 0),
      vec4(0, 0, 1, 0),
      vec4(0, 0, 0, 1)
    );

    uniform float alpha;  
    mat4 rotation = mat4(
      vec4(cos(alpha), sin(alpha), 0, 0),
      vec4(-sin(alpha), cos(alpha), 0, 0),
      vec4(0, 0, 1, 0),
      vec4(0, 0, 0, 1)
    );

    uniform float dx;
    uniform float dy;
    mat4 translation = mat4(
          vec4(1, 0, 0, 0),
          vec4(0, 1, 0, 0),
          vec4(0, 0, 1, 0),
          vec4(dx, dy, 0, 1)
    );

    void main() {
        gl_Position = translation * rotation * scale * vec4(aPos, 1.0);
        vertexColor = aColor;
    })glsl";
  /*fragment shader: takes input from the vertex shader
  and then do the stuff

  nb.:
  FragColor = vec4(...) is actually a cycle
  for fragment in all_fragments:
    FragColor[F] = vec4(...)
  */
  const char *fragmentShaderSource = R"glsl(
    #version 460 core
    in vec4 vertexColor;
    out vec4 FragColor;

    uniform float time;
    void main() {
        FragColor = vec4(
          vertexColor.x + abs(sin(time)),
          vertexColor.y,
          vertexColor.z,
          1.0
        );
    })glsl";

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

int main() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  GLFWwindow *window =
      glfwCreateWindow(1200, 1200, "Rubik's Cube Solver", nullptr, nullptr);
  if (!window) {
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
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

  // telling how to interpret the data inside shaders
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // main loop
  bool isFirst = true;
  float time = 0.0f;
  int alpha = 0;
  auto prev_time = std::chrono::high_resolution_clock::now();
  auto curr_time = prev_time;
  auto timedelta = std::chrono::seconds(1);

  auto alphaID = glGetUniformLocation(shaderProgram, "alpha");
  auto dxID = glGetUniformLocation(shaderProgram, "dx");
  auto dyID = glGetUniformLocation(shaderProgram, "dy");
  auto timeID = glGetUniformLocation(shaderProgram, "time");

  while (!glfwWindowShouldClose(window)) {
    // resize handling
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // uniforms for shaders
    auto dx = (distrib(gen) - 0.5) * 2;
    glUniform1f(dxID, dx);

    auto dy = (distrib(gen) - 0.5) * 2;
    glUniform1f(dyID, dy);

    glUniform1f(timeID, time);
    glUniform1f(alphaID, M_PI * alpha / 180);

    // rendering
    auto curr_time = std::chrono::high_resolution_clock::now();
    if (isFirst || (curr_time - prev_time > timedelta)) {
      do_render(VAO, VBO);
      glfwSwapBuffers(window);
      prev_time = curr_time;
      isFirst = false;
    }
    // update
    time += 0.01f;
    alpha = (alpha >= 360) ? 0 : alpha + 1;
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}