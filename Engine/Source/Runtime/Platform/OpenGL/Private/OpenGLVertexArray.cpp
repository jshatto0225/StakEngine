#include "OpenGLVertexArray.h"

#include "OpenGLBindings.h"
#include "Log.h"

namespace SK
{
    static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
    {
        switch(type)
        {
            case ShaderDataType::FLOAT:    return GL_FLOAT;
            case ShaderDataType::FLOAT2:   return GL_FLOAT;
            case ShaderDataType::FLOAT3:   return GL_FLOAT;
            case ShaderDataType::FLOAT4:   return GL_FLOAT;
            case ShaderDataType::MAT3:     return GL_FLOAT;
            case ShaderDataType::MAT4:     return GL_FLOAT;
            case ShaderDataType::INT:      return GL_INT;
            case ShaderDataType::INT2:     return GL_INT;
            case ShaderDataType::INT3:     return GL_INT;
            case ShaderDataType::INT4:     return GL_INT;
            case ShaderDataType::BOOL:     return GL_BOOL;
            default: SK_CORE_CRITICAL("Unknown Data Type"); return 0;
        }
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &mRendererId);
        mVertexBufferIndex = 0;
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &mRendererId);
    }

    // TODO: This is a mess lol
    void OpenGLVertexArray::AddVertexBuffer(const Shared<VertexBuffer> vbo)
    {
        Bind();
        vbo->Bind();
        const BufferLayout& layout = vbo->GetLayout();
        for(const BufferElement& element : layout)
        {
            switch(element.type)
            {
                case ShaderDataType::FLOAT4:
                {
                    glEnableVertexAttribArray(mVertexBufferIndex);
                    glVertexAttribPointer(mVertexBufferIndex,
                                          element.GetComponentCount(),
                                          ShaderDataTypeToOpenGLType(element.type),
                                          element.normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.offset);
                    mVertexBufferIndex++;
                    break;
                }
                case ShaderDataType::BOOL:
                {
                    glEnableVertexAttribArray(mVertexBufferIndex);
                    glVertexAttribPointer(mVertexBufferIndex,
                                          element.GetComponentCount(),
                                          ShaderDataTypeToOpenGLType(element.type),
                                          element.normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.offset);
                    mVertexBufferIndex++;
                    break;
                }
                case ShaderDataType::MAT4:
                {
                    u8 count = element.GetComponentCount();
                    for(u8 i = 0; i < count; i++)
                    {
                        glEnableVertexAttribArray(mVertexBufferIndex);
                        glVertexAttribPointer(mVertexBufferIndex,
                                              count,
                                              SizeOf(element.type),
                                              element.normalized ? GL_TRUE : GL_FALSE,
                                              layout.GetStride(),
                                              (const void*)(element.offset
                                                            + sizeof(f32)
                                                            * count
                                                            * i));
                        glVertexAttribDivisor(mVertexBufferIndex, 1);
                        mVertexBufferIndex++;
                    }
                }
            }
        }
    }

    void OpenGLVertexArray::SetIndexBuffer(const Shared<IndexBuffer> ibo)
    {
        Bind();
        ibo->Bind();
        mIndexBuffer = ibo;
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(mRendererId);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    const std::vector<Shared<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
    {
        return mVertexBuffers;
    }

    const Shared<IndexBuffer> OpenGLVertexArray::GetIndexBuffer() const
    {
        return mIndexBuffer;
    }
}

