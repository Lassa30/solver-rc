#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    return -1;
  }

  // Create a normal-sized GLFW window
  GLFWwindow *window = glfwCreateWindow(1280, 720, "solver-rc", nullptr,
                                        nullptr); // Not fullscreen
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();

  // Set up ImGui bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // Main render loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Get the current window size (for ImGui to match it)
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set ImGui window size to match GLFW window size (fullscreen within the
    // GLFW window)
    ImGui::SetNextWindowSize(
        ImVec2((float)width, (float)height)); // Fullscreen size
    ImGui::SetNextWindowPos(ImVec2(0, 0));    // Position at top-left
    ImGui::Begin("Main Window", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);

    // Display "Hello, World!" text
    ImGui::Text("Hello, World!");

    ImGui::End();

    // Render ImGui frame
    ImGui::Render();
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // Clear color (background)
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
