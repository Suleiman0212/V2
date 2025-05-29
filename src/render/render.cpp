#include "render/render.hpp"
#include "glad/glad.h"
#include "math/mat4.hpp"
#include "math/vec2.hpp"
#include "math/vec4.hpp"
#include "render/shader.hpp"
#include <array>
#include <cstddef>
#include <vector>

struct Vertex {
  Vec2f pos;
  Vec2f tex_coords;
};

struct InstanceData {
  Mat4f model;
  Vec4f texture_rect;
};

namespace {
const std::array QUAD_VERTICES{
    Vertex(Vec2f(1.0f, 0.0f), Vec2f(1.0f, 0.0f)), // top right
    Vertex(Vec2f(1.0f, 1.0f), Vec2f(1.0f, 1.0f)), // bottom right
    Vertex(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f)), // top left
    Vertex(Vec2f(1.0f, 1.0f), Vec2f(1.0f, 1.0f)), // bottom right
    Vertex(Vec2f(0.0f, 1.0f), Vec2f(0.0f, 1.0f)), // bottom left
    Vertex(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f))  // top left
};
}

Renderer::Renderer(Vec2u canvas_size) {
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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader = Shader::from_file("shaders/vertex.glsl", "shaders/fragment.glsl");

  // Instanced rendering setting up
  glGenBuffers(1, &instanced_vbo_id);
  glBindBuffer(GL_ARRAY_BUFFER, instanced_vbo_id);

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
      2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(Vec4f) * 0));
  glVertexAttribDivisor(2, 1);

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(
      3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(Vec4f) * 1));
  glVertexAttribDivisor(3, 1);

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(
      4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(Vec4f) * 2));
  glVertexAttribDivisor(4, 1);

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(
      5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
      (void *)(offsetof(InstanceData, model) + sizeof(Vec4f) * 3));
  glVertexAttribDivisor(5, 1);

  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)(offsetof(InstanceData, texture_rect)));
  glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);

  shader_instanced = Shader::from_file("shaders/vertex_instanced.glsl",
                                       "shaders/fragment_instanced.glsl");
  set_canvas_size(canvas_size);
}

Renderer::~Renderer() {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &instanced_vbo_id);
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

void Renderer::draw_quads_instanced(std::span<const QuadInstance> quads,
                                    Texture &texture) {
  Shader::bind(&*shader_instanced);
  Texture::bind(&texture);

  Vec2u texture_size = texture.get_size();

  std::vector<InstanceData> instances;
  for (const auto &quad : quads) {
    auto texture_rect =
        Vec4f(quad.texture_rect) /
        Vec4f(texture_size.x, texture_size.y, texture_size.x, texture_size.y);

    instances.emplace_back(quad.model(), texture_rect);
  }

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, instanced_vbo_id);
  glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData),
               instances.data(), GL_STATIC_DRAW);

  glDrawArraysInstanced(GL_TRIANGLES, 0, QUAD_VERTICES.size(), quads.size());
}

void Renderer::set_canvas_size(Vec2u size) {
  Mat4f projection = Mat4f::ortho(0.0f, size.x, size.y, 0.0f);
  shader->set_mat4("projection", projection);
  shader_instanced->set_mat4("projection", projection);
}
