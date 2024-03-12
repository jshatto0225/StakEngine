#pragma once

#include <windows.h>

namespace sk {
/**
 * Platform Data for a GLContext
 */
struct WGLContext {
  HDC device_context = {};
  HGLRC gl_rendering_context = {};
};

} // namespace sk
  //
#define WGL_CONTEXT_STATE WGLContext wgl;

