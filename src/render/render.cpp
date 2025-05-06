#include "render/render.hpp"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"
#include "render/quad.hpp"
#include "render/shader.hpp"

struct Vertex {
  glm::vec2 pos;
  glm::vec2 tex_coords;
};

namespace {
  const std::array QUAD_VERTICES{
    Vertex(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // top right
    Vertex(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),  // bottom right
    Vertex(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)), // top left
    Vertex(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)), // bottom right
    Vertex(glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f)), // bottom left
    Vertex(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f))  // top left
  };
}

Renderer::Renderer(glm::uvec2 canvas_size) {
  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  glBindVertexArray(vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(Vertex), QUAD_VERTICES.data(),
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coords));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader = Shader::from_file("shaders/vertex.glsl", "shaders/fragment.glsl");
  set_canvas_size(canvas_size);
}

Renderer::~Renderer() {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao_id);
  glDeleteBuffers(1, &vbo_id);
}

void Renderer::draw_quad(const Quad &quad) {
  glBindVertexArray(vao_id);
  shader->set_mat4("model", quad.model());
  Texture::bind(&quad.texture);
  glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES.size());
}

void Renderer::draw_quads(std::span<const Quad> quads) {
  for (auto &quad : quads) draw_quad(quad);
}

void Renderer::set_canvas_size(glm::uvec2 size) {
  shader->set_mat4("projection", glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f));
}
