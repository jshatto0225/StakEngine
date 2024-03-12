#pragma once

#ifdef SK_WINDOWS
#include "Win32.h"
#else
#define WIN32_WINDOW_STATE
#define WIN32_PLATFORM_STATE
#endif

#ifdef SK_WGL
#include "WGL.h"
#else
#define WGL_CONTEXT_STATE
#endif

#ifdef SK_X11
#include "X11.h"
#else
#define X11_WINDOW_STATE
#define X11_PLATFORM_STATE
#define X11_CONTEXT_STATE
#endif

#define PLATFORM_WINDOW_STATE \
  WIN32_WINDOW_STATE \
  X11_WINDOW_STATE

#define PLATFORM_STATE \
  WIN32_PLATFORM_STATE \
  X11_PLATFORM_STATE

#define PLATFORM_CONTEXT_STATE \
  WGL_CONTEXT_STATE \
  X11_CONTEXT_STATE

typedef void (*Proc)();

/**
 * Global platform class
 */
class Platform {
public:
  static void Init();
  static void Shutdown();
  static bool IsInitialized();
  static void InitExtensions();

  /**
   * Get function address by name
   */
  static Proc GetProcAddress(const char *name);

private:
  static bool initialized;

public:
  PLATFORM_STATE;
};
