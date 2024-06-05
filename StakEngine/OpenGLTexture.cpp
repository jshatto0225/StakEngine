#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "Texture.h"
#include "Log.h"

///////////////////////
// Private Interface //
///////////////////////

struct texture
{
    u32 RendererId;
    texture_specification Spec;
};

struct texture2d
{
    u32 RendererId;
    texture_specification Spec;
    bool Loaded;
    const char *Path;
};

//////////////////////
// Public Interface //
//////////////////////

texture *
CreateTexture(const texture_specification *TextureSpecification)
{
    texture *Texture = (texture *)malloc(sizeof(texture));

    if (!Texture)
    {
        LogCoreError("Failed to allocate memory for texture");
        return NULL;
    }

    glGenTextures(1, &Texture->RendererId);

    return Texture;
}

void
DestroyTexture(texture **Texture)
{
    if (*Texture != NULL)
    {
        glDeleteTextures(1, &(*Texture)->RendererId);

        free(*Texture);

        *Texture = NULL;
    }
}

const texture_specification *
GetTextureSpecification(const texture *Tex)
{
    return &Tex->Spec;
}

u32
GetTextureWidth(const texture *Tex)
{
    return Tex->Spec.Width;
}

u32
GetTextureHeight(const texture *Tex)
{
    return Tex->Spec.Height;
}

void
SetTextureData(texture *Tex, void *Data, u32 Size)
{

}

void
BindTexture(texture *Tex, u32 Slot)
{

}

bool
IsTextureLoaded(texture *Tex)
{
    return false;
}

bool
CompareTexture(const texture *Tex1, const texture *Tex2)
{
    return Tex1->RendererId == Tex2->RendererId;
}

texture2d *
CreateTexture2D(const char *Path)
{
    texture2d *Tex = (texture2d *)malloc(sizeof(texture2d));

    if (!Tex)
    {
        LogCoreError("Failed to allocate memory for texture 2d");
        return NULL;
    }

    Tex->Loaded = true;
    Tex->Path = Path;

    // LOAD IMAGE
    image *Image;
    Image = CreateImage(Path);
    switch (Image->Data.Channels)
    {
    case 4:
        Tex->Spec.Format = IMAGE_FORMAT_RGBA8;
        break;
    case 3:
        Tex->Spec.Format = IMAGE_FORMAT_RGB8;
        break;
    case 1:
        Tex->Spec.Format = IMAGE_FORMAT_R8;
        break;
    default:
        break;
    }
    image_data ImageData = Image->Data;
    Tex->Spec.Width = ImageData.Width;
    Tex->Spec.Height = ImageData.Height;

    glGenTextures(1, &Tex->RendererId);
    glBindTexture(GL_TEXTURE_2D, Tex->RendererId);
    glTexStorage2D(GL_TEXTURE_2D, 1, ImageFormatToOpenGLInternalFormat(Tex->Spec.Format), Tex->Spec.Width, Tex->Spec.Height);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /* GL_LINEAR */);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Tex->Spec.Width, Tex->Spec.Height, ImageFormatToOpenGLDataFormat(Tex->Spec.Format), GL_UNSIGNED_BYTE, Image->Bytes);

    return Tex;
}

texture2d *
CreateTexture2D(const texture_specification *TextureSpecification)
{
    texture2d *Tex = (texture2d *)malloc(sizeof(texture2d));

    if (!Tex)
    {
        LogCoreError("Failed to allocate memory for texture 2d");
        return NULL;
    }

    Tex->Spec = *TextureSpecification;
    glGenTextures(1, &Tex->RendererId);
    glBindTexture(GL_TEXTURE_2D, Tex->RendererId);
    glTexStorage2D(GL_TEXTURE_2D, 1, ImageFormatToOpenGLInternalFormat(Tex->Spec.Format), Tex->Spec.Width, Tex->Spec.Height);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /* GL_LINEAR */);

    Tex->Loaded = false;

    return Tex;
}

void
DestroyTexture2D(texture2d **Tex)
{
    if (*Tex != NULL)
    {
        glDeleteTextures(1, &(*Tex)->RendererId);

        free(*Tex);

        *Tex = NULL;
    }
}

const texture_specification *
GetTexture2DSpecification(texture2d *Tex)
{
    return &Tex->Spec;
}

u32
GetTexture2DWidth(texture2d *Tex)
{
    return Tex->Spec.Width;
}

u32
GetTexture2DHeight(texture2d *Tex)
{
    return Tex->Spec.Height;
}

u32
GetRendererId(texture2d *Tex)
{
    return Tex->RendererId;
}

const char *
GetTexture2DFiletPath(texture2d *Tex)
{
    return Tex->Path;
}

void
SetTexture2DData(texture2d *Tex, void *Data, u32 Size)
{
    glBindTexture(GL_TEXTURE_2D, Tex->RendererId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Tex->Spec.Width, Tex->Spec.Height, ImageFormatToOpenGLDataFormat(Tex->Spec.Format), GL_UNSIGNED_BYTE, Data);
    Tex->Loaded = true;
}

void
BindTexture2D(const texture2d *Tex, u32 Slot)
{
    //glActiveTexture(GL_TEXTURE0 + slot);
    //glBindTexture(GL_TEXTURE_2D, this->renderer_id);
    glBindTextureUnit(Slot, Tex->RendererId);
}

bool
IsTexture2DLoaded(const texture2d *Tex)
{
    return Tex->Loaded;
}

bool
CompareTexture2D(const texture2d *Tex1, const texture2d *Tex2)
{
    return Tex1->RendererId == Tex2->RendererId;
}

#endif
