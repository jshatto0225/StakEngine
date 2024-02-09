#pragma once

namespace sk {
class Shader {
public:
  Shader() = default;
  virtual ~Shader() {}

  virtual void bind() const = 0;
  virtual void unbind() const = 0;
};
} // namespace SK
