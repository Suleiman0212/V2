#pragma once

#include "render/quad.hpp"
#include "render/shader.hpp"
#include <cstdint>
#include <optional>
#include <span>

class Renderer {
public:
  Renderer(glm::uvec2 canvas_size);
  ~Renderer();

  void draw_quad(const Quad &quad);
  void draw_quads(std::span<const Quad> quads);

  void draw_quad_instanced(std::span<const QuadInstance> quads,
                           Texture &texture);

  void set_canvas_size(glm::uvec2 size);

private:
  std::optional<Shader> shader;
  std::optional<Shader> shader_instanced;
  uint32_t vbo_id, vao_id;
  uint32_t instanced_vbo_id;
};
