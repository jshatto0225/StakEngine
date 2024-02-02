#pragma once

#include <string>

#include "Types.h"

namespace SK
{
    enum class ImageFormat
    {
        R8,
        RGB8,
        RGBA8,
        RGBA32F
    };

    struct TextureSpecification
    {
        u32 width;
        u32 height;
        ImageFormat format;
        bool generateMips;

        TextureSpecification()
        {
            width = 1;
            height = 1;
            format = ImageFormat::RGBA8;
            generateMips = false;
        }
    };

    class Texture
    {
    public:
        Texture() = default;
        virtual ~Texture() {}

        virtual const TextureSpecification& GetSpecification() const = 0;
        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
        virtual u32 GetRendererId() const = 0;
        virtual const std::string& GetPath() const = 0;
        virtual void SetData(void* data, u32 size) = 0;
        virtual void Bind(u32 slot = 0) const = 0;
        virtual bool IsLoaded() const = 0;
        virtual bool operator==(const Texture& other) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        Texture2D() = default;
        virtual ~Texture2D() {}
    };
}

