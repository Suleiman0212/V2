#include "quad.hpp"
#include <glm/ext/matrix_transform.hpp>

Quad::Quad(glm::vec2 pos, glm::vec2 scale, Texture &texture)
    : texture(texture) {
  this->pos = pos;
  this->scale = scale;
  this->origin = scale / 2.0f;
}

glm::mat4 Quad::model() const {
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(pos + origin, 0.0f));
  model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, glm::vec3(-origin, 0.0f));

  model = glm::scale(model, glm::vec3(scale, 1.0f));

  return model;
}
