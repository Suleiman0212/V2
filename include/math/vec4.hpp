#pragma once

#include "misc.hpp"
#include <cmath>

#define VEC_BASE_IMPL(name, T)                                                 \
  constexpr name() : x(0), y(0), z(0), w(0) {}                                 \
  constexpr name(T s) : x(s), y(s), z(s), w(s) {}                              \
  constexpr name(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}               \
                                                                               \
  constexpr name operator-() const { return name(-x, -y, -z, -w); }            \
                                                                               \
  constexpr name operator+(name rhs) const {                                   \
    return name(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);                   \
  }                                                                            \
  constexpr name operator-(name rhs) const {                                   \
    return name(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);                   \
  }                                                                            \
  constexpr name operator*(name rhs) const {                                   \
    return name(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);                   \
  }                                                                            \
  constexpr name operator/(name rhs) const {                                   \
    return name(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);                   \
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

#define VEC_FIELDS(T) T x, y, z, w;

struct Vec4u {
  VEC_BASE_IMPL(Vec4u, unsigned int)
  VEC_FIELDS(unsigned int)
};

struct Vec4i {
  VEC_BASE_IMPL(Vec4i, int)
  VEC_FIELDS(int)
};

struct Vec4f {
  VEC_BASE_IMPL(Vec4f, float)

  constexpr Vec4f(Vec4u rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
  constexpr Vec4f(Vec4i rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

  VEC_FIELDS(float)
};

#undef VEC_FIELDS
#undef VEC_BASE_IMPL

#define VEC_CLAMP_FN(T)                                                        \
  template <> constexpr T clamp(T x, T min, T max) {                           \
    return T(clamp(x.x, min.x, max.x), clamp(x.y, min.y, max.y),               \
             clamp(x.z, min.z, max.z), clamp(x.w, min.w, max.w));              \
  }

namespace math {
VEC_CLAMP_FN(Vec4u)
VEC_CLAMP_FN(Vec4i)
VEC_CLAMP_FN(Vec4f)

inline constexpr float dot(Vec4f a, Vec4f b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline constexpr float length(Vec4f x) { return std::sqrt(dot(x, x)); }
inline constexpr Vec4f normalize(Vec4f x) { return x / length(x); }
} // namespace math

#undef VEC_CLAMP_FN