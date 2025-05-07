#include "render/quad.hpp"
#include <glm/ext/matrix_transform.hpp>

Quad::Quad(glm::vec2 pos, glm::vec2 size, Texture &texture)
    : pos(pos), size(size), origin(size / 2.0f), texture(texture) {}

Quad::Quad(glm::vec2 pos, glm::vec2 size, float angle, glm::vec2 origin,
           Texture &texture)
    : pos(pos), size(size), angle(angle), origin(origin), texture(texture) {}

glm::mat4 Quad::model() const {
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(pos + origin, 0.0f));
  model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, glm::vec3(-origin, 0.0f));

  model = glm::scale(model, glm::vec3(size, 1.0f));

  return model;
}

QuadInstance::QuadInstance(glm::vec2 pos, glm::vec2 size)
    : pos(pos), size(size), origin(size / 2.0f) {}

QuadInstance::QuadInstance(glm::vec2 pos, glm::vec2 size, float angle,
                           glm::vec2 origin)
    : pos(pos), size(size), origin(size / 2.0f) {}

glm::mat4 QuadInstance::model() const {
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(pos + origin, 0.0f));
  model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, glm::vec3(-origin, 0.0f));

  model = glm::scale(model, glm::vec3(size, 1.0f));

  return model;
}
