#define GLFW_INCLUDE_NONE
#include "gfx.hpp"
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "log.hpp"

bool gfx::init() {
    if (glfwGetCurrentContext() == nullptr) {
        trace::error("No OpenGL context active");
        return false;
    }

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        trace::error("Can't load OpenGL 3.3 functions");
        return false;
    }

    return true;
}

void gfx::clear(glm::vec3 color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}