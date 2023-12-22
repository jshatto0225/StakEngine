#include "OpenGLBuffer.h"

OpenGLIndexBuffer::OpenGLIndexBuffer(u32* indices, u32 size)
{

}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{

}

void OpenGLIndexBuffer::Bind() const
{

}

void OpenGLIndexBuffer::Unbind() const
{

}

u32 OpenGLIndexBuffer::GetCount() const
{
    return mCount;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size)
{

}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{

}

void OpenGLVertexBuffer::Bind() const
{

}

void OpenGLVertexBuffer::Unbind() const
{

}

void OpenGLVertexBuffer::SetData(const void* data, u32 size)
{

}

const BufferLayout& OpenGLVertexBuffer::GetLayout() const
{
    return mLayout;
}

void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
{

}
