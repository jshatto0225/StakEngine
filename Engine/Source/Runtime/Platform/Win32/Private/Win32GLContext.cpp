#include "Win32GLContext.h"

#include "Log.h"
#include "OpenGLBindings.h"

namespace sk {
Win32GLContext::Win32GLContext(const Unique<Window> &window) {
  window_handle = (HWND)window->get_handle();
  device_context = GetDC((HWND)window->get_handle());

  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
    PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
    32,            // Colordepth of the framebuffer.
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    24, // Number of bits for the depthbuffer
    8,  // Number of bits for the stencilbuffer
    0,  // Number of Aux buffers in the framebuffer.
    PFD_MAIN_PLANE,
    0,
    0,
    0,
    0
  };
  i32 let_windows_choose_pixel_format = ChoosePixelFormat(device_context, &pfd);
  SetPixelFormat(device_context, let_windows_choose_pixel_format, &pfd);

  gl_rendering_context = wglCreateContext(device_context);
  make_current();
}

Win32GLContext::~Win32GLContext() {
  if (wglGetCurrentContext() == gl_rendering_context) {
    wglMakeCurrent(nullptr, nullptr);
  }
  wglDeleteContext(gl_rendering_context);
  ReleaseDC(window_handle, device_context);
}

void Win32GLContext::make_current() {
  if (!wglMakeCurrent(device_context, gl_rendering_context)) {
    SK_CORE_ERROR("Could not make context current.");
    return;
  }
  bind_gl();
}
} // namespace SK
