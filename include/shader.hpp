#pragma once
#ifndef SHADER_H

#include "glad/glad.h"
#include <cstdint>
#include <string>

class Shader {
public:
  Shader(const char *vertexPath, const char *fragmentPath);
  ~Shader();

  uint32_t program;

  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
};

#endif // !SHADER_H
