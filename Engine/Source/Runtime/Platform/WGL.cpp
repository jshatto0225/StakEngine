#if defined(SK_WGL)

#include <windows.h>
#include <gl/GL.h>

#include "Window.h"
#include "Renderer.h"
#include "Platform.h"

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x0001

typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const i32 *);
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(i32);
typedef BOOL(WINAPI *PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC, i32, i32, u32, const i32 *, i32 *);
typedef BOOL(WINAPI *PFNWGLCHOOSEPIXELFORMATARB)(HDC, const i32 *, const f32 *, u32, i32 *, u32 *);

PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
PFNWGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB;

Proc Platform::GetProcAddress(const char *name) {
  return (Proc)wglGetProcAddress(name);
}

void Platform::InitExtensions() {
  PIXELFORMATDESCRIPTOR pfd;
  HGLRC dummy_hglrc;
  HGLRC current_hglrc = wglGetCurrentContext();
  HDC current_dc = wglGetCurrentDC();
  HDC dummy_dc;

  dummy_dc = GetDC(Platform::win32.dummy_window);
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;

  SetPixelFormat(dummy_dc, ChoosePixelFormat(dummy_dc, &pfd), &pfd);
  dummy_hglrc = wglCreateContext(dummy_dc);
  wglMakeCurrent(dummy_dc, dummy_hglrc);

  wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
  wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
  wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
  wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARB)wglGetProcAddress("wglChoosePixelFormatARB");

  wglMakeCurrent(current_dc, current_hglrc);
}

Context::Context(const Window *win) {
  if (!win) {
    return;
  }
  this->window = win;
  this->wgl.device_context = GetDC(win->data.win32.handle);

  PIXELFORMATDESCRIPTOR pfd = {
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

  const int pixel_format_attrib_list[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0 // End of attributes list
  };
  int attributes[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 5,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      WGL_CONTEXT_FLAGS_ARB, 
      WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 0
  };

  int pixel_format = 0;
  u32 num_formats = 0;

  wglChoosePixelFormatARB(this->wgl.device_context,
                          pixel_format_attrib_list,
                          nullptr,
                          1,
                          &pixel_format,
                          &num_formats);

  this->wgl.gl_rendering_context = wglCreateContextAttribsARB(this->wgl.device_context, 0, attributes);

  SetPixelFormat(this->wgl.device_context, pixel_format, &pfd);

  this->wgl.gl_rendering_context = wglCreateContextAttribsARB(this->wgl.device_context, 0, attributes);
  wglMakeCurrent(this->wgl.device_context, this->wgl.gl_rendering_context);

  RenderApi::Bind();
}

Context::~Context() {
  if (wglGetCurrentContext() == this->wgl.gl_rendering_context) {
    wglMakeCurrent(nullptr, nullptr);
  }
  wglDeleteContext(this->wgl.gl_rendering_context);
}

void Context::makeCurrent() {
  wglMakeCurrent(this->wgl.device_context, this->wgl.gl_rendering_context);
  RenderApi::Bind();
}

void Context::swapBuffers() {
  SwapBuffers(this->wgl.device_context);
}

#endif
