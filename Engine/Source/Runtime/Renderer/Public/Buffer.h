#pragma once

#include <vector>
#include <string>

#include "Types.h"

namespace SK
{
    enum class ShaderDataType
    {
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        MAT3,
        MAT4,
        INT,
        INT2,
        INT3,
        INT4,
        BOOL
    };

    inline u32 SizeOf(ShaderDataType type)
    {
        switch(type)
        {
            case ShaderDataType::FLOAT:     return 4;
            case ShaderDataType::FLOAT2:    return 4 * 2;
            case ShaderDataType::FLOAT3:    return 4 * 3;
            case ShaderDataType::FLOAT4:    return 4 * 4;
            case ShaderDataType::MAT3:      return 4 * 3 * 3;
            case ShaderDataType::MAT4:      return 4 * 4 * 4;
            case ShaderDataType::INT:       return 4;
            case ShaderDataType::INT2:      return 4 * 2;
            case ShaderDataType::INT3:      return 4 * 3;
            case ShaderDataType::INT4:      return 4 * 4;
            case ShaderDataType::BOOL:      return 1;
        }
        return 0;
    }

    struct BufferElement
    {
        std::string name;
        ShaderDataType type;
        u32 size;
        u64 offset;
        bool normalized;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized)
        {
            this->name = name;
            this->type = type;
            this->normalized = normalized;
            this->offset = 0;
            this->size = SizeOf(type);
        }

        u32 GetComponentCount() const
        {
            switch(type)
            {
                case ShaderDataType::FLOAT: return 1;
                case ShaderDataType::FLOAT2: return 2;
                case ShaderDataType::FLOAT3: return 3;
                case ShaderDataType::FLOAT4: return 4;
                case ShaderDataType::MAT3: return 3;
                case ShaderDataType::MAT4: return 4;
                case ShaderDataType::INT: return 1;
                case ShaderDataType::INT2: return 2;
                case ShaderDataType::INT3: return 3;
                case ShaderDataType::INT4: return 4;
                case ShaderDataType::BOOL: return 1;
                default: return 0;
            }
        }
    };

    class BufferLayout final
    {
    public:
        inline BufferLayout()
        {
            mStride = 0;
        }

        BufferLayout(std::initializer_list<BufferElement> elements)
        {
            mElements = elements;
            u64 offset = 0;
            mStride = 0;
            for(BufferElement& element : mElements)
            {
                element.offset = offset;
                offset += element.size;
                mStride += element.size;
            }
        }

        inline u32 GetStride() const { return mStride; }
        inline const std::vector<BufferElement> GetElements() const { return mElements; }

        std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
        std::vector<BufferElement>::iterator end() { return mElements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }

    private:
        std::vector<BufferElement> mElements;
        u32 mStride;
    };

    class IndexBuffer
    {
        public:
        IndexBuffer() = default;
        virtual ~IndexBuffer() {};

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual u32 GetCount() const = 0;
    };

    class VertexBuffer
    {
        public:
        VertexBuffer() = default;
        virtual ~VertexBuffer() {}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void SetData(const void* data, u32 size) = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;
    };
}

