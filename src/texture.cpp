#include "log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "include/texture.hpp"
#include <format>
#include <glad/glad.h>

const unsigned char fallback_texture[] = {255, 0, 255, 255, 0,   0, 0,   255,
                                          0,   0, 0,   255, 255, 0, 255, 255};

Texture::Texture(const char *path) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(true);
  int nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D); // Не нужен в 2D
    stbi_image_free(data);
  } else {
    trace::error(std::format("Failed to load texture: {}", path));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 fallback_texture);
    // glGenerateMipmap(GL_TEXTURE_2D); // Также не нужен
  }
}
