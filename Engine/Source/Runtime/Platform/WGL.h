#pragma once

#include <windows.h>

/**
 * Platform Data for a GLContext
 */
struct WGLContext {
  HDC device_context = {};
  HGLRC gl_rendering_context = {};
};

#define WGL_CONTEXT_STATE WGLContext wgl;