#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>

class Texture {
public:
  static void bind(Texture *texture);

  Texture(const std::string &filename);
  ~Texture();

  bool load(const std::string &filename);
  void update(glm::uvec2 size, const uint8_t *pixels);

  glm::uvec2 get_size() const;

private:
  uint32_t id = 0;
  glm::uvec2 size;
};
