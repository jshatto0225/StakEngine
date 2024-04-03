#include "WGLPlatform.h"

#ifdef SK_WGL

#include "Window.h"
#include "Renderer.h"
#include "Log.h"

WGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;
WGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
WGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
WGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
WGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
WGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB;
WGLCREATECONTEXTPROC wglCreateContext;
WGLDELETECONTEXTPROC wglDeleteContext;
WGLGETCURRENTDCPROC wglGetCurrentDC;
WGLGETPROCADDRESSPROC wglGetProcAddress;
WGLGETCURRENTCONTEXTPROC wglGetCurrentContext;
WGLMAKECURRENTPROC wglMakeCurrent;

void *
PlatformGetProcAddress(const char *name)
{
  return (void *)wglGetProcAddress(name);
}

void
PlatformInitExtensions()
{
  LoadRenderApiLibrary();

  PIXELFORMATDESCRIPTOR PFD;
  HGLRC DummyHGLRC;
  HGLRC CurrentHGLRC = wglGetCurrentContext();
  HDC CurrentHDC = wglGetCurrentDC();
  HDC DummyHDC;

  DummyHDC = GetDC(Platform.DummyWindow);
  PFD.nSize = sizeof(PFD);
  PFD.nVersion = 1;
  PFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  PFD.iPixelType = PFD_TYPE_RGBA;
  PFD.cColorBits = 24;

  i32 Choose = ChoosePixelFormat(DummyHDC, &PFD);
  SetPixelFormat(DummyHDC, Choose, &PFD);
  DummyHGLRC = wglCreateContext(DummyHDC);
  wglMakeCurrent(DummyHDC, DummyHGLRC);

  wglGetExtensionsStringEXT = (WGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
  wglGetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
  wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  wglSwapIntervalEXT = (WGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  wglGetPixelFormatAttribivARB = (WGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
  wglChoosePixelFormatARB = (WGLCHOOSEPIXELFORMATARB)wglGetProcAddress("wglChoosePixelFormatARB");

  wglMakeCurrent(CurrentHDC, CurrentHGLRC);
}

void
LoadRenderApiLibrary()
{
  Platform.RendererApi = LoadLibraryA("OpenGL32.dll");
  if (!Platform.RendererApi)
  {
    LogCoreError("Failed to load OpenGL");
  }

  wglCreateContext = (WGLCREATECONTEXTPROC)GetRendererApiProc("wglCreateContext");
  wglDeleteContext = (WGLDELETECONTEXTPROC)GetRendererApiProc("wglDeleteContext");
  wglGetCurrentDC = (WGLGETCURRENTDCPROC)GetRendererApiProc("wglGetCurrentDC");
  wglGetProcAddress = (WGLGETPROCADDRESSPROC)GetRendererApiProc("wglGetProcAddress");
  wglGetCurrentContext = (WGLGETCURRENTCONTEXTPROC)GetRendererApiProc("wglGetCurrentContext");
  wglMakeCurrent = (WGLMAKECURRENTPROC)GetRendererApiProc("wglMakeCurrent");
}

void *
GetRendererApiProc(const char *Name)
{
  return GetProcAddress(Platform.RendererApi, Name);
}

#endif
