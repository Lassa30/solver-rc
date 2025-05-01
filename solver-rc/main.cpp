#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

// clang-format off
#include <GL/glxmd.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <chrono>
#include <iostream>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distrib(0, 1);

static void glfw_error_callback(int error, const char *description) {
  std::cout << (stderr, "GLFW Error %d: %s\n", error, description);
}

float r1 = 0;
float g1 = 1;
float b1 = 0;

float r2 = 1;
float g2 = 0;
float b2 = 0;
float x = 0.1f;

float Vertices[3][7] = {{-x, x, 0, r1, g1, b1, 1},
                        {x, x, 0, 0, 0, 0, 1},
                        {x, -x, 0, r2, g2, b2, 1}};

void do_render(GLuint VAO, GLuint VBO) {
  glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
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

  /*scale-rotate-translate vertexShader*/
  const char *vertexShaderSource = R"glsl(
    #version 460 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    out vec4 vertexColor;
    
    uniform float time;
    float k = abs(sin(time)) / 2;
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

  GLint success_vertex;
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success_vertex);
  if (!success_vertex) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "Vertex shader error:\n" << infoLog << std::endl;
  }

  GLint success_fragment;
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success_fragment);
  if (!success_fragment) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "Vertex shader error:\n" << infoLog << std::endl;
  }

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
      glfwCreateWindow(1200, 1200, "Rubik's cube", nullptr, nullptr);
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

  /*telling how to interpret the data inside shaders*/
  // clang-format off
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // clang-format on

  bool isFirst = true;
  float time = 0.0f;
  int alpha = 0;
  auto prev_time = std::chrono::high_resolution_clock::now();
  auto curr_time = prev_time;
  auto timedelta = std::chrono::seconds(1);

  while (!glfwWindowShouldClose(window)) {
    // clang-format off
    /*set uniforms
    btw: don't do glGetUniformLocation in the loop
    */
    glUseProgram(shaderProgram);

    auto dx = distrib(gen);
    glUniform1f(glGetUniformLocation(shaderProgram, "dx"), dx);

    auto dy = distrib(gen);
    glUniform1f(glGetUniformLocation(shaderProgram, "dy"), dy);

    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), M_PI * alpha / 180);

    /*rendering*/
    auto curr_time = std::chrono::high_resolution_clock::now();
    if (isFirst || (curr_time - prev_time > timedelta)) {
      do_render(VAO, VBO);
      glfwSwapBuffers(window);
      prev_time = curr_time;
      isFirst = false;
    }
    /*update uniforms*/
    time += 0.01f;
    alpha = (alpha >= 360) ? 0 : alpha + 1;
    /*pool*/
    glfwPollEvents();
    // clang-format on
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}