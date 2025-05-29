#pragma once

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
  constexpr auto operator<=>(const name &rhs) const = default;                 \
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

  constexpr float dot(Vec4f rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
  }

  constexpr float len() const { return std::sqrt(dot(*this)); }
  constexpr Vec4f normalize() const { return *this / len(); }

  constexpr Vec4f lerp(Vec4f rhs, float t) const {
    return Vec4f(std::lerp(x, rhs.x, t), std::lerp(y, rhs.y, t),
                 std::lerp(z, rhs.z, t), std::lerp(w, rhs.w, t));
  }

  VEC_FIELDS(float)
};

#undef VEC_FIELDS
#undef VEC_BASE_IMPL