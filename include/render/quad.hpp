#pragma once

#include <glm/glm.hpp>
#include "texture.hpp"

struct Quad {
  Quad(glm::vec2 pos, glm::vec2 size, Texture &texture);
  Quad(glm::vec2 pos, glm::vec2 size, float angle, glm::vec2 origin, Texture &texture);

  glm::mat4 model() const;

  glm::vec2 pos;
  glm::vec2 size{1.0f};
  float angle = 0.0f;
  glm::vec2 origin;
  Texture &texture;
};
