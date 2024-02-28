#pragma once

#include <windows.h>

struct WGLContext {
  HDC device_context = {};
  HGLRC gl_rendering_context = {};
};

#define WGL_CONTEXT_STATE WGLContext wgl;