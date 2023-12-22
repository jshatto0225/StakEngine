#pragma once

#include "Buffer.h"

class VertexArray
{
public:
    VertexArray() = default;
    virtual ~VertexArray() {}

    virtual void AddVertexBuffer(const Shared<VertexBuffer> vbo) = 0;
    virtual void SetIndexBuffer(const Shared<IndexBuffer> ibo) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const = 0;
    virtual const Shared<IndexBuffer> GetIndexBuffer() const = 0;
};
