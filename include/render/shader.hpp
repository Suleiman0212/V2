#pragma once

#include "../math/mat4.hpp"
#include "../math/vec2.hpp"
#include "../math/vec3.hpp"
#include "../math/vec4.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

class Shader {
public:
  static std::optional<Shader> from_string(std::string_view vertex_source,
                                           std::string_view frag_source);
  static std::optional<Shader> from_file(const std::string &vertex_filename,
                                         const std::string &frag_filename);

  static void bind(Shader *shader);

  Shader(const Shader &rhs) = delete;
  Shader(Shader &&rhs);
  ~Shader();

  Shader &operator=(const Shader &rhs) = delete;
  Shader &operator=(Shader &&rhs);

  void set_bool(const std::string &name, bool value);
  void set_int(const std::string &name, int value);
  void set_float(const std::string &name, float value);
  void set_vec2(const std::string &name, Vec2f value);
  void set_vec3(const std::string &name, Vec3f value);
  void set_vec4(const std::string &name, Vec4f value);
  void set_mat4(const std::string &name, Mat4f value);

private:
  Shader(uint32_t id);

  uint32_t id = 0;
};
