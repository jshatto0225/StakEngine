#pragma once

#include "Buffer.h"

namespace SK
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(u32* indices, u32 size);
        ~OpenGLIndexBuffer();

        void Bind() const override;
        void Unbind() const override;

        u32 GetCount() const override;

    private:
        u32 mRendererId;
        u32 mCount;
    };

    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(u32 size);
        ~OpenGLVertexBuffer();

        void Bind() const override;
        void Unbind() const override;
        void SetData(const void* data, u32 size) override;

        const BufferLayout& GetLayout() const override;
        void SetLayout(const BufferLayout& layout) override;

    private:
        u32 mRendererId;
        BufferLayout mLayout;
    };
}

