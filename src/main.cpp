#include <cstdio>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <type_traits>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

#include "document.hpp"
#include "graph.hpp"
#include "utils.hpp"
#include "gui.hpp"

// Dear ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main() {

  // Setup Window
  glfwSetErrorCallback(glfw_error_callback);
  if( !glfwInit() )
    return 1;

  GLFWwindow* window = glfwCreateWindow(720, 720, "Dear ImGui Test", NULL, NULL);
  if( !window )
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Vsync

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();


  docgraph testdir;
  testdir.scan_dir("test_dir");
  
  for( auto doc : testdir.getDoc("REGS") ){
    auto refs = doc->parseReferences<DOCTYPE::WORD_XML>();
  }

  // testdir.parseAndConnect();
  
  static bool close = false;

  // My Program
  while( !glfwWindowShouldClose(window) && !close){
    glfwPollEvents();

    // Start ImGUI Frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // Actual Drawing Part
    close = menuBar();

    close |= referenceWindow(testdir);



    // Render Section
    ImGui::Render();
    int disp_w, disp_h;
    glfwGetFramebufferSize(window, &disp_w, &disp_h);
    glViewport(0, 0, disp_w, disp_h);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
  }

  // Shutdown ImGUI and Backend
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL2_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();


}
