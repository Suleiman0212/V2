#include "render/texture.hpp"
#include <format>
#include <utility>
#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "log.hpp"
#include <stb/stb_image.h>

namespace {
constexpr uint8_t MISSING_TEXTURE_PIXELS[] = {
    0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff,
    0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff,
};

Texture *binding = nullptr;
} // namespace

void Texture::bind(Texture *texture) {
  if (texture == binding)
    return;
  glBindTexture(GL_TEXTURE_2D, (texture != nullptr) ? texture->id : 0);
  binding = texture;
}

Texture::Texture(const std::string &filename) {
  glGenTextures(1, &id);
  bind(this);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  load(filename);
}

Texture::Texture(Texture &&rhs) { *this = std::move(rhs); }

Texture::~Texture() {
  if (this == binding)
    bind(nullptr);
  if (id != 0)
    glDeleteTextures(1, &id);
}

Texture &Texture::operator=(Texture &&rhs) {
  if (&rhs != this) {
    std::swap(rhs.id, id);
    std::swap(rhs.size, size);
    if (&rhs == binding)
      binding = this;
  }
  return *this;
}

bool Texture::load(const std::string &filename) {
  stbi_set_flip_vertically_on_load(false);

  int width, height;
  stbi_uc *pixels = stbi_load(filename.c_str(), &width, &height, nullptr, 4);
  if (pixels != nullptr) {
    update(glm::uvec2(width, height), pixels);
    stbi_image_free(pixels);
    return true;
  } else {
    trace::error(std::format("Failed to load texture: {}", filename));
    update(glm::uvec2(2, 2), MISSING_TEXTURE_PIXELS);
    return false;
  }
}

void Texture::update(glm::uvec2 size, const uint8_t *pixels) {
  bind(this);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);
  this->size = size;
}

glm::uvec2 Texture::get_size() const { return size; }
