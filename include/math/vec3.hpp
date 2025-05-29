#pragma once

#include "misc.hpp"
#include <cmath>

#define VEC_BASE_IMPL(name, T)                                                 \
  constexpr name() : x(0), y(0), z(0) {}                                       \
  constexpr name(T s) : x(s), y(s), z(s) {}                                    \
  constexpr name(T x, T y, T z) : x(x), y(y), z(z) {}                          \
                                                                               \
  constexpr name operator-() const { return name(-x, -y, -z); }                \
                                                                               \
  constexpr name operator+(name rhs) const {                                   \
    return name(x + rhs.x, y + rhs.y, z + rhs.z);                              \
  }                                                                            \
  constexpr name operator-(name rhs) const {                                   \
    return name(x - rhs.x, y - rhs.y, z - rhs.z);                              \
  }                                                                            \
  constexpr name operator*(name rhs) const {                                   \
    return name(x * rhs.x, y * rhs.y, z * rhs.z);                              \
  }                                                                            \
  constexpr name operator/(name rhs) const {                                   \
    return name(x / rhs.x, y / rhs.y, z / rhs.z);                              \
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

#define VEC_FIELDS(T) T x, y, z;

struct Vec3u {
  VEC_BASE_IMPL(Vec3u, unsigned int)
  VEC_FIELDS(unsigned int)
};

struct Vec3i {
  VEC_BASE_IMPL(Vec3i, int)
  VEC_FIELDS(int)
};

struct Vec3f {
  VEC_BASE_IMPL(Vec3f, float)

  constexpr Vec3f(Vec3u rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
  constexpr Vec3f(Vec3i rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

  VEC_FIELDS(float)
};

#undef VEC_FIELDS
#undef VEC_BASE_IMPL

#define VEC_CLAMP_FN(T)                                                        \
  template <> constexpr T clamp(T x, T min, T max) {                           \
    return T(clamp(x.x, min.x, max.x), clamp(x.y, min.y, max.y),               \
             clamp(x.z, min.z, max.z));                                        \
  }

namespace math {
VEC_CLAMP_FN(Vec3u)
VEC_CLAMP_FN(Vec3i)
VEC_CLAMP_FN(Vec3f)

inline constexpr float dot(Vec3f a, Vec3f b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline constexpr Vec3f cross(Vec3f a, Vec3f b) {
  return Vec3f(a.y * b.z - a.z * b.y, -(a.x * b.z - a.z * b.x),
               a.x * b.y - a.y * b.x);
}

inline constexpr float length(Vec3f x) { return std::sqrt(dot(x, x)); }
inline constexpr Vec3f normalize(Vec3f x) { return x / length(x); }
} // namespace math

#undef VEC_CLAMP_FN