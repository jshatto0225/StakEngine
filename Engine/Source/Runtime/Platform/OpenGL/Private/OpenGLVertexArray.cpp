#include "OpenGLVertexArray.h"

OpenGLVertexArray::OpenGLVertexArray()
{

}

OpenGLVertexArray::~OpenGLVertexArray()
{

}

void OpenGLVertexArray::AddVertexBuffer(const Shared<VertexBuffer> vbo)
{

}

void OpenGLVertexArray::SetIndexBuffer(const Shared<IndexBuffer> ibo)
{

}

void OpenGLVertexArray::Bind() const
{

}

void OpenGLVertexArray::Unbind() const
{

}

const std::vector<Shared<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
{
    return mVertexBuffers;
}

const Shared<IndexBuffer> OpenGLVertexArray::GetIndexBuffer() const
{
    return mIndexBuffer;
}
