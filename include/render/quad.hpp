#pragma once

#include "../math/mat4.hpp"
#include "../math/vec2.hpp"
#include "../math/vec4.hpp"
#include "texture.hpp"

struct Quad {
  Quad(Vec2f pos, Vec2f size, Texture &texture);
  Quad(Vec2f pos, Vec2f size, float angle, Vec2f origin, Texture &texture);

  Mat4f model() const;

  Vec2f pos;
  Vec2f size{1.0f};
  float angle = 0.0f;
  Vec2f origin;
  Texture &texture;
};

struct QuadInstance {
  QuadInstance(Vec2f pos, Vec2f size, Vec4i texture_rect);
  QuadInstance(Vec2f pos, Vec2f size, float angle, Vec2f origin,
               Vec4i texture_rect);

  Mat4f model() const;

  Vec2f pos;
  Vec2f size{1.0f};
  float angle = 0.0f;
  Vec2f origin;
  Vec4i texture_rect;
};
