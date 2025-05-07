#include "render/render.hpp"
#include "glad/glad.h"
#include "render/shader.hpp"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct Vertex {
  glm::vec2 pos;
  glm::vec2 tex_coords;
};

struct InstanceData {
  glm::mat4 model;
};

namespace {
const std::array QUAD_VERTICES{
    Vertex(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // top right
    Vertex(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)), // bottom right
    Vertex(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)), // top left
    Vertex(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)), // bottom right
    Vertex(glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f)), // bottom left
    Vertex(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f))  // top left
};
}

Renderer::Renderer(glm::uvec2 canvas_size) {
  // Ordinary rendering setting up
  glGenVertexArrays(1, &vao_id);
  glGenBuffers(1, &vbo_id);
  glBindVertexArray(vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(Vertex),
               QUAD_VERTICES.data(), GL_STATIC_DRAW);

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

  // Instanced rendering setting up
  glGenBuffers(1, &instanced_vbo_id);
  glBindBuffer(GL_ARRAY_BUFFER, instanced_vbo_id);

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
      2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(glm::vec4) * 0));
  glVertexAttribDivisor(2, 1);

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(
      3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(glm::vec4) * 1));
  glVertexAttribDivisor(3, 1);

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(
      4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(glm::vec4) * 2));
  glVertexAttribDivisor(4, 1);

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(
      5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(glm::vec4) * 3));
  glVertexAttribDivisor(5, 1);

  shader_instanced = Shader::from_file("shaders/vertex_instanced.glsl",
                                       "shaders/fragment_instanced.glsl");
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
  for (auto &quad : quads)
    draw_quad(quad);
}

void Renderer::draw_quad_instanced(std::span<const QuadInstance> quads,
                                   Texture &texture) {
  Texture::bind(&texture);
  std::vector<InstanceData> instances;
  for (auto &quad : quads) {
    instances.emplace_back(InstanceData{quad.model()});
  }

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, instanced_vbo_id);
  glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData),
               instances.data(), GL_STATIC_DRAW);
  glDrawArraysInstanced(GL_TRIANGLES, 0, QUAD_VERTICES.size(), quads.size());
}

void Renderer::set_canvas_size(glm::uvec2 size) {
  shader->set_mat4("projection",
                   glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f));
}
