#include "Win32GLContext.h"

#include "Log.h"
#include "OpenGLBindings.h"

Win32GLContext::Win32GLContext(const Unique<Window>& window)
{
    mWindowHandle = (HWND)window->GetHandle();
    mDeviceContext = GetDC((HWND)window->GetHandle());

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    i32 letWindowsChoosePixelFormat = ChoosePixelFormat(mDeviceContext, &pfd);
    SetPixelFormat(mDeviceContext, letWindowsChoosePixelFormat, &pfd);

    mGLRenderingContext = wglCreateContext(mDeviceContext);
    MakeCurrent();
}

Win32GLContext::~Win32GLContext()
{
    if(wglGetCurrentContext() == mGLRenderingContext)
    {
        wglMakeCurrent(nullptr, nullptr);
    }
    wglDeleteContext(mGLRenderingContext);
    ReleaseDC(mWindowHandle, mDeviceContext);
}

void Win32GLContext::MakeCurrent()
{
    if(!wglMakeCurrent(mDeviceContext, mGLRenderingContext))
    {
        SK_CORE_ERROR("Could not make context current.");
        return;
    }
    BindGL();
}

