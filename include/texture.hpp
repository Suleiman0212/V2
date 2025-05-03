#include "include/stb_image/stb_image.h"
#include <cstdint>

class Texture {
public:
  Texture(const char *path);
  uint32_t id;
  int width, height;
};
