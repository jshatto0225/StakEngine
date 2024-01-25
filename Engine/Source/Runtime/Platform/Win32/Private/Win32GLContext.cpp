#include "Win32GLContext.h"

#include "Log.h"
#include "WindowManager.h"
#include "OpenGLBindings.h"

Win32GLContext::Win32GLContext(u64 window)
{
    mWindow = window;
    mDeviceContext = GetDC((HWND)WindowManager::GetWindowHandle(window));
    mGLRenderingContext = wglCreateContext(mDeviceContext);
    MakeCurrent();
}

void Win32GLContext::MakeCurrent()
{
    if(!wglMakeCurrent(mDeviceContext, mGLRenderingContext))
    {
        SK_CORE_ERROR("Could not make context current.");
    }
    BindGL();
}

