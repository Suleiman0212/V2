#include "render/quad.hpp"
#include "math/mat4.hpp"
#include "math/misc.hpp"
#include "math/vec2.hpp"
#include "math/vec4.hpp"

Quad::Quad(Vec2f pos, Vec2f size, Texture &texture)
    : pos(pos), size(size), origin(size / 2.0f), texture(texture) {}

Quad::Quad(Vec2f pos, Vec2f size, float angle, Vec2f origin, Texture &texture)
    : pos(pos), size(size), angle(angle), origin(origin), texture(texture) {}

Mat4f Quad::model() const {
  Mat4f model(1.0f);
  model *= Mat4f::translation(pos + origin);
  model *= Mat4f::rotation_z(math::rad(angle));
  model *= Mat4f::translation(-origin);
  model *= Mat4f::scaling(size);
  return model;
}

QuadInstance::QuadInstance(Vec2f pos, Vec2f size, Vec4i texture_rect)
    : pos(pos), size(size), origin(size / 2.0f), texture_rect(texture_rect) {}

QuadInstance::QuadInstance(Vec2f pos, Vec2f size, float angle, Vec2f origin,
                           Vec4i texture_rect)
    : pos(pos), size(size), origin(size / 2.0f), texture_rect(texture_rect) {}

Mat4f QuadInstance::model() const {
  Mat4f model(1.0f);
  model *= Mat4f::translation(pos + origin);
  model *= Mat4f::rotation_z(math::rad(angle));
  model *= Mat4f::translation(-origin);
  model *= Mat4f::scaling(size);
  return model;
}
