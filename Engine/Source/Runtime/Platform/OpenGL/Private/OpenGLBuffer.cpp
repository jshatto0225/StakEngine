#include "OpenGLBuffer.h"

#include "OpenGLBindings.h"

OpenGLIndexBuffer::OpenGLIndexBuffer(u32* indices, u32 size)
{
    mCount = size;
    glGenBuffers(1, &mRendererId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(u32), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &mRendererId);
}

void OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererId);
}

void OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 OpenGLIndexBuffer::GetCount() const
{
    return mCount;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size)
{
    glGenBuffers(1, &mRendererId);
    glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &mRendererId);
}

void OpenGLVertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
}

void OpenGLVertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const void* data, u32 size)
{
    glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

const BufferLayout& OpenGLVertexBuffer::GetLayout() const
{
    return mLayout;
}

void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
{
    mLayout = layout;
}
