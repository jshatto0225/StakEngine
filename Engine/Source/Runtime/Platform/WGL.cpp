#if defined(SK_WGL)

#include <windows.h>

#include "Window.h"
#include "Renderer.h"

namespace sk {
Context::Context(Window *win) {
  if (!win) {
    return;
  }
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

  int pixel_format = ChoosePixelFormat(this->wgl.device_context, &pfd);
  SetPixelFormat(this->wgl.device_context, pixel_format, &pfd);

  this->wgl.gl_rendering_context = wglCreateContext(this->wgl.device_context);
  wglMakeCurrent(this->wgl.device_context, this->wgl.gl_rendering_context);
}

Context::~Context() {
  if (wglGetCurrentContext() == this->wgl.gl_rendering_context) {
    wglMakeCurrent(nullptr, nullptr);
  }
  wglDeleteContext(this->wgl.gl_rendering_context);
}

void Context::make_current() {
  wglMakeCurrent(this->wgl.device_context, this->wgl.gl_rendering_context);
  sk_bind_renderer();
}
} // namespace sk

#endif