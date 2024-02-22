#pragma once

#include <windows.h>

struct _SKWGLContext {
  HDC device_context = {};
  HGLRC gl_rendering_context = {};
};

#define SK_WGL_CONTEXT_STATE _SKWGLContext wgl;