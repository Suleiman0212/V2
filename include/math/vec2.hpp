#pragma once

#include <cmath>

#define VEC_BASE_IMPL(name, T)                                                 \
  constexpr name() : x(0), y(0) {}                                             \
  constexpr name(T s) : x(s), y(s) {}                                          \
  constexpr name(T x, T y) : x(x), y(y) {}                                     \
                                                                               \
  constexpr name operator-() const { return name(-x, -y); }                    \
                                                                               \
  constexpr name operator+(name rhs) const {                                   \
    return name(x + rhs.x, y + rhs.y);                                         \
  }                                                                            \
  constexpr name operator-(name rhs) const {                                   \
    return name(x - rhs.x, y - rhs.y);                                         \
  }                                                                            \
  constexpr name operator*(name rhs) const {                                   \
    return name(x * rhs.x, y * rhs.y);                                         \
  }                                                                            \
  constexpr name operator/(name rhs) const {                                   \
    return name(x / rhs.x, y / rhs.y);                                         \
  }                                                                            \
                                                                               \
  constexpr name operator*(T s) const { return *this * name(s); }              \
  constexpr name operator/(T s) const { return *this / name(s); }              \
                                                                               \
  constexpr auto operator<=>(const name &rhs) const = default;                 \
                                                                               \
  constexpr name &operator+=(name rhs) { return *this = *this + rhs; }         \
  constexpr name &operator-=(name rhs) { return *this = *this - rhs; }         \
  constexpr name &operator*=(name rhs) { return *this = *this * rhs; }         \
  constexpr name &operator/=(name rhs) { return *this = *this / rhs; }         \
                                                                               \
  constexpr name &operator*=(T s) { return *this *= name(s); }                 \
  constexpr name &operator/=(T s) { return *this /= name(s); }

#define VEC_FIELDS(T) T x, y;

struct Vec2u {
  VEC_BASE_IMPL(Vec2u, unsigned int)
  VEC_FIELDS(unsigned int)
};

struct Vec2i {
  VEC_BASE_IMPL(Vec2i, int)
  VEC_FIELDS(int)
};

struct Vec2f {
  VEC_BASE_IMPL(Vec2f, float)

  constexpr Vec2f(Vec2u rhs) : x(rhs.x), y(rhs.y) {}
  constexpr Vec2f(Vec2i rhs) : x(rhs.x), y(rhs.y) {}

  constexpr float dot(Vec2f rhs) const { return x * rhs.x + y * rhs.y; }
  constexpr float perp_dot(Vec2f rhs) const { return x * rhs.y - y * rhs.x; }

  constexpr float len() const { return std::sqrt(dot(*this)); }
  constexpr Vec2f normalize() const { return *this / len(); }

  constexpr Vec2f lerp(Vec2f rhs, float t) const {
    return Vec2f(std::lerp(x, rhs.x, t), std::lerp(y, rhs.y, t));
  }

  constexpr float angle_to(Vec2f rhs) const {
    return std::atan2(rhs.y - y, rhs.x - x);
  }

  VEC_FIELDS(float)
};

#undef VEC_FIELDS
#undef VEC_BASE_IMPL