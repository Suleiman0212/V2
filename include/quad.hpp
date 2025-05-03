#pragma once
#include "texture.hpp"
#include <glm/glm.hpp>

class Quad {
public:
  Quad(glm::vec2 pos, glm::vec2 scale, Texture &texture);

  glm::mat4 model() const;

  glm::vec2 pos;
  float angle = 0;
  glm::vec2 scale;
  glm::vec2 origin = glm::vec2(0, 0);
  Texture &texture;
};
