#pragma once

#include "VertexArray.h"

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    void AddVertexBuffer(const Shared<VertexBuffer> vbo) override;
    void SetIndexBuffer(const Shared<IndexBuffer> ibo) override;
    void Bind() const override;
    void Unbind() const override;

    const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const override;
    const Shared<IndexBuffer> GetIndexBuffer() const override;

private:
    std::vector<Shared<VertexBuffer>> mVertexBuffers;
    Shared<IndexBuffer> mIndexBuffer;
};
