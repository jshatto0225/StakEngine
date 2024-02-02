#pragma once

#include "Texture.h"

namespace SK
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const TextureSpecification& textureSpecification);
        ~OpenGLTexture();

        const TextureSpecification& GetSpecification() const override;
        u32 GetWidth() const override;
        u32 GetHeight() const override;
        u32 GetRendererId() const override;
        const std::string& GetPath() const override;
        void SetData(void* data, u32 size) override;
        void Bind(u32 slot = 0) const override;
        bool IsLoaded() const override;
        bool operator==(const Texture& other) const override;

    private:
        TextureSpecification mTextureSpecification;
        std::string mPath;
        u32 mRendererId;
    };

    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const TextureSpecification& textureSpecification);
        ~OpenGLTexture2D();

        const TextureSpecification& GetSpecification() const override;
        u32 GetWidth() const override;
        u32 GetHeight() const override;
        u32 GetRendererId() const override;
        const std::string& GetPath() const override;
        void SetData(void* data, u32 size) override;
        void Bind(u32 slot = 0) const override;
        bool IsLoaded() const override;
        bool operator==(const Texture& other) const override;

    private:
        TextureSpecification mTextureSpecification;
        std::string mPath;
        u32 mRendererId;
    };
}

