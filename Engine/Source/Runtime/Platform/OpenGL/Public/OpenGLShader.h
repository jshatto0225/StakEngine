#pragma once

#include <string>

#include "Shader.h"

namespace sk {
class OpenGLShader : public Shader {
public:
  OpenGLShader(const std::string &vs, const std::string &fs);
  ~OpenGLShader();

  void bind() const override;
  void unbind() const override;
};
} // namespace SK
