#include "WGLPlatform.h"

#ifdef SK_WGL

#include "RenderCommand.h"
#include "Log.h"
#include "Window.h"

/*********************
 * Private Interface *
 *********************/

struct context
{
    const window *Window;
    HDC DeviceContext;
    HGLRC GLRenderingContext;
};

/********************
 * Public Interface *
 ********************/

context *
CreateContext(const window *Window)
{
    context *Context = (context *)malloc(sizeof(context));
    
    if (!Context)
    {
        LogCoreError("Failed to allocate memory for context");
        return NULL;
    }
    
    Context->Window = Window;
    Context->DeviceContext = GetDC(Window->Handle);
    
    PIXELFORMATDESCRIPTOR PFD =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            16,  // depth buffer
            0,   // stencil buffer
            0,   // auxiliary buffer
            PFD_MAIN_PLANE,
            0,   // reserved
            0, 0, 0
        };
    
    const int PixelFormatAttribList[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0 // End of attributes list
        };
    int Attributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 0
        };
    
    int PixelFormat = 0;
    u32 NumFormats = 0;
    
    wglChoosePixelFormatARB(Context->DeviceContext, PixelFormatAttribList, NULL, 1, &PixelFormat, &NumFormats);
    
    SetPixelFormat(Context->DeviceContext, PixelFormat, &PFD);
    
    Context->GLRenderingContext = wglCreateContextAttribsARB(Context->DeviceContext, 0, Attributes);
    wglMakeCurrent(Context->DeviceContext, Context->GLRenderingContext);
    
    RenderCommandBind();
    
    return Context;
}

void
DestroyContext(context **Context)
{
    if (*Context) {
        if (wglGetCurrentContext() == (*Context)->GLRenderingContext)
        {
            wglMakeCurrent(NULL, NULL);
        }
        wglDeleteContext((*Context)->GLRenderingContext);
        
        free(*Context);
        
        *Context = NULL;
    }
}

void
MakeContextCurrent(const context *Context)
{
    wglMakeCurrent(Context->DeviceContext, Context->GLRenderingContext);
    RenderCommandBind();
}

void
SwapContextBuffers(const context *Context)
{
    SwapBuffers(Context->DeviceContext);
}

#endif
