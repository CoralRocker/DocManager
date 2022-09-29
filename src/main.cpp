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

using std::cout, std::endl;

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
  


  // testdir.parseAndConnect();
  
  static bool close = false;
  bool parsed = false;

  // My Program
  while( !glfwWindowShouldClose(window) && !close){
    glfwPollEvents();

    // Start ImGUI Frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if( !parsed ){ // Parse/Load Documents First
      parsed = parseDocs(testdir);
    }else{ // Do work on parsed docs
      // Actual Drawing Part
      close = menuBar();

      close |= referenceWindow(testdir);
    }


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

  for( unsigned i = 0; i < testdir.size(); i++  ){
    cout << "Printing Out BFS For Document " << testdir.getChild(i)->docname() << endl;
    for( auto it = testdir.bfsbegin(i); it != testdir.bfsend(); ++it ){
      cout << "\t" << it->docname() << endl;
    }
  }

  // cout << "Printing out BFS iteration over document 0" << endl;
  // for( auto it = testdir.bfsbegin(0); it != testdir.bfsend(); ++it ){
  //   cout << it->filename() << endl;
  // }

  // Shutdown ImGUI and Backend
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL2_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();


}
