#if defined(SK_WGL)

#include <windows.h>

#include "Window.h"
#include "Renderer.h"

void _sk_create_context(_SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->wgl.device_context = GetDC(internal_window->win32.handle);
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

  int pixel_format = ChoosePixelFormat(internal_window->wgl.device_context, &pfd);
  SetPixelFormat(internal_window->wgl.device_context, pixel_format, &pfd);

  internal_window->wgl.gl_rendering_context = wglCreateContext(internal_window->wgl.device_context);
}

void sk_make_context_current(SKWindow *win) {
  if (!win) {
    wglMakeCurrent(nullptr, nullptr);
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  wglMakeCurrent(internal_window->wgl.device_context, internal_window->wgl.gl_rendering_context);
  sk_bind_renderer();
}

SKProc sk_get_proc_address(const char *proc) {
  return (SKProc)wglGetProcAddress(proc);
}

void sk_window_swap_buffers(SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  SwapBuffers(internal_window->wgl.device_context);
}

#endif