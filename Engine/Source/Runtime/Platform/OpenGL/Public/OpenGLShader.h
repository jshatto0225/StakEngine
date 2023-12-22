#pragma once

#include <string>

#include "Shader.h"

class OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& vs, const std::string& fs);
    ~OpenGLShader();

    void Bind() const override;
    void Unbind() const override;
};