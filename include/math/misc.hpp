#pragma once

#include <numbers>

namespace math {
static constexpr double PI = std::numbers::pi;

template <class T> constexpr T clamp(T x, T min, T max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

template <class T> constexpr T lerp(T a, T b, float t) {
  return a * (1.0f - t) + b * t;
}

template <class T> constexpr T rad(T deg) { return deg * (PI / 180.0); };
template <class T> constexpr T deg(T rad) { return rad * (180.0 / PI); };
} // namespace math