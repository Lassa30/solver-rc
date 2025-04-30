#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <random>

// random setup
std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<> distrib(0, 1);

static void glfw_error_callback(int error, const char *description) {
  std::cout << (stderr, "GLFW Error %d: %s\n", error, description);
}

void do_render() {
  auto x = distrib(gen);
  auto r = distrib(gen);
  auto g = distrib(gen);
  auto b = distrib(gen);
  auto a = distrib(gen);

  glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_QUADS);
  glColor4f(r, g, b, a);
  glVertex3f(-x, x, 0);
  glVertex3f(x, x, 0);
  glVertex3f(x, -x, 0);
  glVertex3f(-x, -x, 0);
  glEnd();
}

int main() {
  // glfw setup
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return 1;
  }
  GLFWwindow *window =
      glfwCreateWindow(800, 800, "Rubik's cube", nullptr, nullptr);
  if (!window) {
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // fix: it is not working properly, when I do it with glad...
  // int version = gladLoadGL(glfwGetProcAddress);

  // it seems useless until everything is happening inside the window
  // glViewport(0, 0, 400, 300);

  // Main render loop
  auto previous_time = std::chrono::high_resolution_clock::now();
  const std::chrono::milliseconds render_interval(3000);

  std::cout << previous_time << '\n';
  bool isFirstFrame = true;
  while (!glfwWindowShouldClose(window)) {
    auto current_time = std::chrono::high_resolution_clock::now();
    if (isFirstFrame || (current_time - previous_time >= render_interval)) {
      do_render();
      glfwSwapBuffers(window);

      previous_time = current_time;
      isFirstFrame = false;
    }

    // Pool
    glfwPollEvents();
  }

  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}