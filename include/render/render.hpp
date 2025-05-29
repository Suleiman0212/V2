#pragma once

#include "../math/vec2.hpp"
#include "quad.hpp"
#include "shader.hpp"
#include <cstdint>
#include <optional>
#include <span>

class Renderer {
public:
  Renderer(Vec2u canvas_size);
  ~Renderer();

  void draw_quad(const Quad &quad);
  void draw_quads(std::span<const Quad> quads);

  void draw_quads_instanced(std::span<const QuadInstance> quads,
                            Texture &texture);

  void set_canvas_size(Vec2u size);

private:
  std::optional<Shader> shader;
  std::optional<Shader> shader_instanced;
  uint32_t vbo_id, vao_id;
  uint32_t instanced_vbo_id;
};
