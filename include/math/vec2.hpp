#pragma once

#include "misc.hpp"
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
  constexpr bool operator==(const name &rhs) const = default;                  \
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

  VEC_FIELDS(float)
};

#undef VEC_FIELDS
#undef VEC_BASE_IMPL

#define VEC_CLAMP_FN(T)                                                        \
  template <> constexpr T clamp(T x, T min, T max) {                           \
    return T(clamp(x.x, min.x, max.x), clamp(x.y, min.y, max.y));              \
  }

namespace math {
VEC_CLAMP_FN(Vec2u)
VEC_CLAMP_FN(Vec2i)
VEC_CLAMP_FN(Vec2f)

inline constexpr float dot(Vec2f a, Vec2f b) { return a.x * b.x + a.y * b.y; }
inline constexpr float perp_dot(Vec2f a, Vec2f b) {
  return a.x * b.y - a.y * b.x;
}

inline constexpr float length(Vec2f x) { return std::sqrt(dot(x, x)); }
inline constexpr Vec2f normalize(Vec2f x) { return x / length(x); }

inline constexpr float angle_to(Vec2f from, Vec2f to) {
  return std::atan2(to.y - from.y, to.x - from.x);
}
} // namespace math

#undef VEC_CLAMP_FN