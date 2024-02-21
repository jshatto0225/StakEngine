#pragma once

class Shader {
public:
  Shader() = default;
  virtual ~Shader() {}

  virtual void bind() const = 0;
  virtual void unbind() const = 0;
};

