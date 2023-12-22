#include "OpenGLTexture.h"
#include "Texture.h"

OpenGLTexture::OpenGLTexture(const TextureSpecification& textureSpecification)
{

}

OpenGLTexture::~OpenGLTexture()
{

}

const TextureSpecification& OpenGLTexture::GetSpecification() const
{
    return mTextureSpecification;
}

u32 OpenGLTexture::GetWidth() const
{
    return mTextureSpecification.width;
}

u32 OpenGLTexture::GetHeight() const
{
    return mTextureSpecification.height;
}

u32 OpenGLTexture::GetRendererId() const
{
    return mRendererId;
}

const std::string& OpenGLTexture::GetPath() const
{
    return mPath;
}

void OpenGLTexture::SetData(void* data, u32 size)
{
    
}

void OpenGLTexture::Bind(u32 slot) const
{

}

bool OpenGLTexture::IsLoaded() const
{
    return false;
}

bool OpenGLTexture::operator==(const Texture& other) const
{
    return mRendererId == other.GetRendererId();
}

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& textureSpecification)
{

}

OpenGLTexture2D::~OpenGLTexture2D()
{

}

const TextureSpecification& OpenGLTexture2D::GetSpecification() const
{
    return mTextureSpecification;
}

u32 OpenGLTexture2D::GetWidth() const
{
    return mTextureSpecification.width;
}

u32 OpenGLTexture2D::GetHeight() const
{
    return mTextureSpecification.height;
}

u32 OpenGLTexture2D::GetRendererId() const
{
    return mRendererId;
}

const std::string& OpenGLTexture2D::GetPath() const
{
    return mPath;
}

void OpenGLTexture2D::SetData(void* data, u32 size)
{
    
}

void OpenGLTexture2D::Bind(u32 slot) const
{

}

bool OpenGLTexture2D::IsLoaded() const
{
    return false;
}

bool OpenGLTexture2D::operator==(const Texture& other) const
{
    return mRendererId == other.GetRendererId();
}

