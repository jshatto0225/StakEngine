#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "Framebuffer.h"
#include "Log.h"

///////////////////////
// Private Interface //
///////////////////////

struct framebuffer
{
    u32 RendererId;
    u32 Width;
    u32 Height;
    u32 ColorAttachment;
    u32 DepthAttachment;
};

//////////////////////
// Public Interface //
//////////////////////

framebuffer *
CreateFramebuffer(const framebuffer_spec *Spec)
{
    framebuffer *Framebuffer = (framebuffer *)malloc(sizeof(framebuffer));
    
    if (!Framebuffer)
    {
        LogCoreError("Failed to allocate memory for framebuffer");
        return NULL;
    }
    
    Framebuffer->Width = Spec->Width;
    Framebuffer->Height = Spec->Height;
    
    glCreateFramebuffers(1, &Framebuffer->RendererId);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer->RendererId);
    
    glGenTextures(1, &Framebuffer->ColorAttachment);
    glBindTexture(GL_TEXTURE_2D, Framebuffer->ColorAttachment);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 Spec->Width,
                 Spec->Height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Framebuffer->ColorAttachment, 0);
    
    glGenTextures(1, &Framebuffer->DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, Framebuffer->DepthAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Spec->Width, Spec->Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, Spec->Width, Spec->Height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, Framebuffer->DepthAttachment, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LogCoreError("Framebuffer not complete");
        return NULL;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return Framebuffer;
}

void
BindFramebuffer(const framebuffer *Framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer->RendererId);
}

void
UnbindFramebuffer(const framebuffer *Framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
DestroyFramebuffer(framebuffer **Framebuffer)
{
    if (*Framebuffer)
    {
        glDeleteTextures(1, &(*Framebuffer)->DepthAttachment);
        glDeleteTextures(1, &(*Framebuffer)->ColorAttachment);
        glDeleteFramebuffers(1, &(*Framebuffer)->RendererId);
        free(*Framebuffer);
        *Framebuffer = NULL;
    }
}

void
ResizeFramebuffer(framebuffer *Framebuffer, const framebuffer_spec *Spec)
{
}

#endif
