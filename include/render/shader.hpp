#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <string_view>

class Shader {
public:
  static std::optional<Shader> from_string(std::string_view vertex_source, std::string_view frag_source);
  static std::optional<Shader> from_file(const std::string &vertex_filename, const std::string &frag_filename);

  static void bind(Shader *shader);

  Shader(const Shader &rhs) = delete;
  Shader(Shader &&rhs);
  ~Shader();

  Shader &operator=(Shader &&rhs);

  void set_bool(const std::string &name, bool value);
  void set_int(const std::string &name, int value);
  void set_float(const std::string &name, float value);
  void set_vec2(const std::string &name, glm::vec2 value);
  void set_vec3(const std::string &name, glm::vec3 value);
  void set_vec4(const std::string &name, glm::vec4 value);
  void set_mat4(const std::string &name, glm::mat4 value);

private:
  Shader(uint32_t id);

  uint32_t id = 0;
};
