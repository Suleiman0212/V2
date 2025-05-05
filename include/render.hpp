#pragma once
#include "glad/glad.h"
#include "quad.hpp"
#include "shader.hpp"
#include "window.hpp"
#include <vector>

class Renderer {
public:
  Renderer(Window &window);
  ~Renderer();

  void draw_quad(const Quad &quad);
  void draw_quads(const std::vector<Quad> &quads);
  void update_projection(const Window &window);

private:
  Shader shader;
  GLuint VBO, VAO;
};
