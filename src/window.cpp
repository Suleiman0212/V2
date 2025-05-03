#include "include/window.hpp"
#include "include/glad/glad.h"
#include "log.hpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *glfw_handle, int width, int height) {
  Window *window = (Window *)glfwGetWindowUserPointer(glfw_handle);
  glViewport(0, 0, width, height);
  if (window->framebuffer_callback != nullptr)
    window->framebuffer_callback(width, height);
}

Window::Window(std::string title, uint32_t width, uint32_t height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    trace::error("Failed to create window");
    glfwTerminate();
  }
  glfwSetWindowUserPointer(window, this);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    trace::error("Failed to initialize opengl functions");
  }
}

Window::~Window() { glfwTerminate(); }

bool Window::running() {
  events();
  return !glfwWindowShouldClose(window);
}

void Window::clear(glm::vec4 color) const {
  glClearColor(color.r, color.g, color.b, 1.0f);
  glfwSetWindowOpacity(window, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::is_key_down(int key) {
  return glfwGetKey(window, key) == GLFW_PRESS;
}

void Window::get_framebuffer_size(int &width, int &height) const {
  glfwGetFramebufferSize(window, &width, &height);
}

void Window::events() {
  glfwSwapBuffers(window);
  glfwPollEvents();
}
