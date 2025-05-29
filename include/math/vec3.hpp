#pragma once

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
  constexpr auto operator<=>(const name &rhs) const = default;                 \
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

  constexpr float dot(Vec3f rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }
  constexpr Vec3f cross(Vec3f rhs) const {
    return Vec3f(y * rhs.z - z * rhs.y, -(x * rhs.z - z * rhs.x),
                 x * rhs.y - y * rhs.x);
  }

  constexpr float len() const { return std::sqrt(dot(*this)); }
  constexpr Vec3f normalize() const { return *this / len(); }

  constexpr Vec3f lerp(Vec3f rhs, float t) const {
    return Vec3f(std::lerp(x, rhs.x, t), std::lerp(y, rhs.y, t),
                 std::lerp(z, rhs.z, t));
  }

  VEC_FIELDS(float)
};

#undef VEC_FIELDS
#undef VEC_BASE_IMPL