#include "render/shader.hpp"
#include <format>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string_view>
#include <utility>
#include "glad/glad.h"
#include "log.hpp"
#include "util.hpp"

namespace {
  Shader *binding = nullptr;
}

std::optional<Shader> Shader::from_string(std::string_view vertex_source, std::string_view frag_source) {
  auto create_shader = [](GLenum type, std::string_view type_str, std::string_view source) -> GLuint {
    GLuint id = glCreateShader(type);
    const GLchar *source_ptr = source.data();
    GLint source_len = source.size();
    glShaderSource(id, 1, &source_ptr, &source_len);
    glCompileShader(id);

    GLint log_len;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len != 0) {
      auto log = std::make_unique<char[]>(log_len);
      glGetShaderInfoLog(id, log_len, nullptr, log.get());
      trace::info(std::format("Compilation log of {} shader:\n{}", type_str, std::string_view(log.get(), log_len)));
    }

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (!status) {
      // compilation failed
      trace::error(std::format("Failed to compile {} shader", type_str));
      glDeleteShader(id);
      return 0;
    }
    return id;
  };

  GLuint vertex_id = create_shader(GL_VERTEX_SHADER, "vertex", vertex_source);
  GLuint frag_id = create_shader(GL_FRAGMENT_SHADER, "fragment", frag_source);
  if (vertex_id == 0 || frag_id == 0) return std::nullopt;

  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, vertex_id);
  glAttachShader(program_id, frag_id);
  glLinkProgram(program_id);

  glDetachShader(program_id, frag_id);
  glDeleteShader(frag_id);
  glDetachShader(program_id, vertex_id);
  glDeleteShader(vertex_id);

  return Shader(program_id);
}

std::optional<Shader> Shader::from_file(const std::string &vertex_filename, const std::string &frag_filename) {
  auto vertex_source = util::read_file(vertex_filename);
  auto frag_source = util::read_file(frag_filename);
  if (!vertex_source || !frag_source) return std::nullopt;

  return from_string(std::string_view(vertex_source->begin(), vertex_source->end()), 
                    std::string_view(frag_source->begin(), frag_source->end()));
}

void Shader::bind(Shader *shader) {
  if (shader == binding) return;
  glUseProgram((shader != nullptr)? shader->id : 0);
  binding = shader;
}

Shader::Shader(Shader &&rhs) {
  *this = std::move(rhs);
}

Shader::~Shader() {
  if (this == binding) bind(nullptr);
  if (id != 0) glDeleteProgram(id);
}

Shader &Shader::operator=(Shader &&rhs) {
  if (&rhs != this) {
    std::swap(id, rhs.id);
  }
  return *this;
}

void Shader::set_bool(const std::string &name, bool value) {
  bind(this);
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set_int(const std::string &name, int value) {
  bind(this);
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) {
  bind(this);
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, glm::vec2 value) {
  bind(this);
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string &name, glm::vec3 value) {
  bind(this);
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec4(const std::string &name, glm::vec4 value) {
  bind(this);
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_mat4(const std::string &name, glm::mat4 value) {
  bind(this);
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

Shader::Shader(uint32_t id): id(id) {}