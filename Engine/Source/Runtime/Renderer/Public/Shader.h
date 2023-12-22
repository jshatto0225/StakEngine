#pragma once

class Shader
{
public:
    Shader() = default;
    virtual ~Shader() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
};
