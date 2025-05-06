#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include "render/shader.hpp"
#include "render/quad.hpp"

class Renderer {
public:
  Renderer(glm::uvec2 canvas_size);
  ~Renderer();

  void draw_quad(const Quad &quad);
  void draw_quads(std::span<const Quad> quads);

  void set_canvas_size(glm::uvec2 size);

private:
  std::optional<Shader> shader;
  uint32_t vbo_id, vao_id;
};
