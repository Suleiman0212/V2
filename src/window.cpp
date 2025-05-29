#include "window.hpp"
#include "glad/glad.h"
#include "log.hpp"
#include "math/vec2.hpp"
#include "math/vec4.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *glfw_handle, int width, int height) {
  Window *window = (Window *)glfwGetWindowUserPointer(glfw_handle);
  glViewport(0, 0, width, height);
  if (window->framebuffer_callback != nullptr)
    window->framebuffer_callback(width, height);
}

Window::Window(const std::string &title, Vec2u size) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    trace::error("Failed to create window");
    glfwTerminate();
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    trace::error("Failed to initialize opengl functions");
  }
}

Window::~Window() { glfwTerminate(); }

bool Window::is_running() const {
  glfwPollEvents();
  return !glfwWindowShouldClose(window);
}

void Window::clear(Vec4f color) const {
  glClearColor(color.x, color.y, color.z, 1.0f);
  glfwSetWindowOpacity(window, color.w);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Window::present() const { glfwSwapBuffers(window); }

bool Window::is_key_down(int key) {
  return glfwGetKey(window, key) == GLFW_PRESS;
}

void Window::get_framebuffer_size(int &width, int &height) const {
  glfwGetFramebufferSize(window, &width, &height);
}