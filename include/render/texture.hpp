#pragma once

#include "../math/vec2.hpp"
#include <cstdint>
#include <string>

class Texture {
public:
  static void bind(Texture *texture);

  Texture(const std::string &filename);
  Texture(const Texture &rhs) = delete;
  Texture(Texture &&rhs);
  ~Texture();

  Texture &operator=(const Texture &rhs) = delete;
  Texture &operator=(Texture &&rhs);

  bool load(const std::string &filename);
  void update(Vec2u size, const uint8_t *pixels);

  Vec2u get_size() const;

private:
  uint32_t id = 0;
  Vec2u size;
};
