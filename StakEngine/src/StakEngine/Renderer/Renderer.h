#pragma once

#include "glad/glad.h"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
};