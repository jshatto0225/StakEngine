#pragma once

#ifdef SK_WINDOWS
#include "Win32Window.h"
#include "Win32Platform.h"
#include "WGLContext.h"
#else
#define SK_WIN32_WINDOW_STATE
#define SK_WIN32_PLATFORM_STATE
#error Platform Not Supported
#endif

#define SK_PLATFORM_WINDOW_STATE \
  SK_WIN32_WINDOW_STATE

#define SK_PLATFORM_STATE \
  SK_WIN32_PLATFORM_STATE

#define SK_PLATFORM_CONTEXT_STATE \
  SK_WGL_CONTEXT_STATE

struct _SKPlatform {
  bool initialized = false;
  SK_PLATFORM_STATE
};

void sk_platform_init();
void sk_platform_shutdown();

typedef void (*SKProc)();
SKProc sk_get_proc_address(const char *);

extern _SKPlatform _sk;

