#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include <cmath>

struct Mat4f {
  // clang-format off
  static constexpr Mat4f translation(Vec2f pos) {
    return Mat4f(1.0f,  0.0f,  0.0f, 0.0f, 
                 0.0f,  1.0f,  0.0f, 0.0f, 
                 0.0f,  0.0f,  1.0f, 0.0f, 
                 pos.x, pos.y, 0.0f, 1.0f);
  }

  static constexpr Mat4f translation(Vec3f pos) {
    return Mat4f(1.0f,  0.0f,  0.0f,  0.0f,
                 0.0f,  1.0f,  0.0f,  0.0f,
                 0.0f,  0.0f,  1.0f,  0.0f,
                 pos.x, pos.y, pos.z, 1.0f);
  }

  static constexpr Mat4f rotation_z(float angle) {
    float s = std::sin(angle), c = std::cos(angle);
    return Mat4f(c,   -s,    0.0f, 0.0f, 
                 s,    c,    0.0f, 0.0f, 
                 0.0f, 0.0f, 1.0f, 0.0f, 
                 0.0f, 0.0f, 0.0f, 1.0f);
  }

  static constexpr Mat4f scaling(Vec2f scale) {
    return Mat4f(scale.x, 0.0f,    0.0f, 0.0f,
                 0.0f,    scale.y, 0.0f, 0.0f,
                 0.0f,    0.0f,    1.0f, 0.0f,
                 0.0f,    0.0f,    0.0f, 1.0f);
  }

  static constexpr Mat4f scaling(Vec3f scale) {
    return Mat4f(scale.x, 0.0f,    0.0f,    0.0f,
                 0.0f,    scale.y, 0.0f,    0.0f,
                 0.0f,    0.0f,    scale.z, 0.0f,
                 0.0f,    0.0f,    0.0f,    1.0f);
  }

  static constexpr Mat4f ortho(float t, float r, float b, float l, float near = 0.0f, float far = 1.0f) {
    float m00 = 2.0f / (r - l);
    float m11 = 2.0f / (t - b);
    float m22 = -2.0f / (far - near);

    float m30 = -(r + l) / (r - l);
    float m31 = -(t + b) / (t - b);
    float m32 = -(far + near) / (far - near);
    return Mat4f(m00,  0.0f, 0.0f, 0.0f,
                 0.0f, m11,  0.0f, 0.0f,
                 0.0f, 0.0f, m22,  0.0f,
                 m30,  m31,  m32,  1.0f);
  }

  constexpr Mat4f()
      : m00(0.0f), m01(0.0f), m02(0.0f), m03(0.0f), 
        m10(0.0f), m11(0.0f), m12(0.0f), m13(0.0f),
        m20(0.0f), m21(0.0f), m22(0.0f), m23(0.0f),
        m30(0.0f), m31(0.0f), m32(0.0f), m33(0.0f) {}
  constexpr Mat4f(float d)
      : m00(d),    m01(0.0f), m02(0.0f), m03(0.0f),
        m10(0.0f), m11(d),    m12(0.0f), m13(0.0f),
        m20(0.0f), m21(0.0f), m22(d),    m23(0.0f),
        m30(0.0f), m31(0.0f), m32(0.0f), m33(d) {}
  constexpr Mat4f(
          float m00, float m01, float m02, float m03, 
          float m10, float m11, float m12, float m13, 
          float m20, float m21, float m22, float m23, 
          float m30, float m31, float m32, float m33)
          : m00(m00), m01(m01), m02(m02), m03(m03), 
            m10(m10), m11(m11), m12(m12), m13(m13),
            m20(m20), m21(m21), m22(m22), m23(m23),
            m30(m30), m31(m31), m32(m32), m33(m33) {}
  // clang-format on

  constexpr Mat4f operator*(Mat4f rhs) const {
    Mat4f r;

    r.m00 = m00 * rhs.m00 + m10 * rhs.m01 + m20 * rhs.m02 + m30 * rhs.m03;
    r.m01 = m01 * rhs.m00 + m11 * rhs.m01 + m21 * rhs.m02 + m31 * rhs.m03;
    r.m02 = m02 * rhs.m00 + m12 * rhs.m01 + m22 * rhs.m02 + m32 * rhs.m03;
    r.m03 = m03 * rhs.m00 + m13 * rhs.m01 + m23 * rhs.m02 + m33 * rhs.m03;

    r.m10 = m00 * rhs.m10 + m10 * rhs.m11 + m20 * rhs.m12 + m30 * rhs.m13;
    r.m11 = m01 * rhs.m10 + m11 * rhs.m11 + m21 * rhs.m12 + m31 * rhs.m13;
    r.m12 = m02 * rhs.m10 + m12 * rhs.m11 + m22 * rhs.m12 + m32 * rhs.m13;
    r.m13 = m03 * rhs.m10 + m13 * rhs.m11 + m23 * rhs.m12 + m33 * rhs.m13;

    r.m20 = m00 * rhs.m20 + m10 * rhs.m21 + m20 * rhs.m22 + m30 * rhs.m23;
    r.m21 = m01 * rhs.m20 + m11 * rhs.m21 + m21 * rhs.m22 + m31 * rhs.m23;
    r.m22 = m02 * rhs.m20 + m12 * rhs.m21 + m22 * rhs.m22 + m32 * rhs.m23;
    r.m23 = m03 * rhs.m20 + m13 * rhs.m21 + m23 * rhs.m22 + m33 * rhs.m23;

    r.m30 = m00 * rhs.m30 + m10 * rhs.m31 + m20 * rhs.m32 + m30 * rhs.m33;
    r.m31 = m01 * rhs.m30 + m11 * rhs.m31 + m21 * rhs.m32 + m31 * rhs.m33;
    r.m32 = m02 * rhs.m30 + m12 * rhs.m31 + m22 * rhs.m32 + m32 * rhs.m33;
    r.m33 = m03 * rhs.m30 + m13 * rhs.m31 + m23 * rhs.m32 + m33 * rhs.m33;

    return r;
  }

  constexpr Vec2f operator*(Vec2f rhs) const {
    Vec2f r;
    r.x = m00 * rhs.x + m10 * rhs.y;
    r.y = m01 * rhs.x + m11 * rhs.y;
    return r;
  }

  constexpr Vec3f operator*(Vec3f rhs) const {
    Vec3f r;
    r.x = m00 * rhs.x + m10 * rhs.y + m20 * rhs.z;
    r.y = m01 * rhs.x + m11 * rhs.y + m21 * rhs.z;
    r.z = m02 * rhs.x + m12 * rhs.y + m22 * rhs.z;
    return r;
  }

  constexpr Vec4f operator*(Vec4f rhs) const {
    Vec4f r;
    r.x = m00 * rhs.x + m10 * rhs.y + m20 * rhs.z + m30 * rhs.w;
    r.y = m01 * rhs.x + m11 * rhs.y + m21 * rhs.z + m31 * rhs.w;
    r.z = m02 * rhs.x + m12 * rhs.y + m22 * rhs.z + m32 * rhs.w;
    r.w = m03 * rhs.x + m13 * rhs.y + m23 * rhs.z + m33 * rhs.w;
    return r;
  }

  constexpr Mat4f operator*=(Mat4f rhs) { return *this = *this * rhs; }

  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
};