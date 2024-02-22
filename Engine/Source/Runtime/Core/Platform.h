#pragma once

#ifdef SK_WINDOWS
#include "Win32.h"
#else
#define SK_WIN32_WINDOW_STATE
#define SK_WIN32_PLATFORM_STATE
#endif

#ifdef SK_WGL
#include "WGL.h"
#else
#define SK_WGL_CONTEXT_STATE
#endif

#ifdef SK_X11
#include "X11.h"
#else
#define SK_X11_WINDOW_STATE
#define SK_X11_PLATFORM_STATE
#endif

#define SK_PLATFORM_WINDOW_STATE \
  SK_WIN32_WINDOW_STATE \
  SK_X11_WINDOW_STATE

#define SK_PLATFORM_STATE \
  SK_WIN32_PLATFORM_STATE \
  SK_X11_PLATFORM_STATE

#define SK_PLATFORM_CONTEXT_STATE \
  SK_WGL_CONTEXT_STATE \
  SK_X11_CONTEXT_STATE

struct _SKWindow;

struct _SKPlatform {
  bool initialized = false;
  _SKWindow *window_head = nullptr;
  _SKWindow *window_last = nullptr;

  SK_PLATFORM_STATE;
};

void sk_platform_init();
void sk_platform_shutdown();

typedef void (*SKProc)();
SKProc sk_get_proc_address(const char *);
