#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distrib(0, 1);

static void glfw_error_callback(int error, const char *description) {
  std::cout << (stderr, "GLFW Error %d: %s\n", error, description);
}

float r1 = distrib(gen);
float g1 = distrib(gen);
float b1 = distrib(gen);

float r2 = distrib(gen);
float g2 = distrib(gen);
float b2 = distrib(gen);
float x = 0.5f;

float Vertices[4][7] = {{-x, x, 0, r1, g1, b1, 1},
                        {x, x, 0, 0, 1, 0, 1},
                        {x, -x, 0, 0, 0, 1, 1},
                        {-x, -x, 0, r2, b2, g2, 1}};

void do_render(GLuint VAO, GLuint VBO) {
  glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

GLuint compileShaders() {
  const char *vertexShaderSource = R"glsl(
    #version 460 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    out vec4 vertexColor;
    void main() {
        gl_Position = vec4(aPos, 1.0);
        vertexColor = aColor;
    })glsl";

  const char *fragmentShaderSource = R"glsl(
    #version 460 core
    in vec4 vertexColor;
    out vec4 FragColor;

    uniform float time;

    void main() {
        FragColor = vec4(
          vertexColor.x - abs(2 * cos(time) * sin(time)),
          vertexColor.y + abs(cos(time + 0.77)),
          vertexColor.z + abs(sin(time)),
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
  std::cout << "THE COLORS:\nfirst: " << r1 << ' ' << g1 << ' ' << b1
            << "\nsecond: " << r2 << ' ' << g2 << ' ' << b2 << '\n';

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(800, 800, "Rubik's cube", nullptr, nullptr);
  if (!window)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  /*compiling shaders*/
  auto shaderProgram = compileShaders();

  /*init buffer*/
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

  glUseProgram(shaderProgram);
  auto currTime = glGetUniformLocation(shaderProgram, "time");
  float time = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    time += 0.01f;

    // Set shader uniforms
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);

    do_render(VAO, VBO);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}