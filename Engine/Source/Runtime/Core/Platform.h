#pragma once

#ifdef SK_WINDOWS
#include "Win32Window.h"
#else
#define SK_WIN32_WINDOW_STATE
#error Platform Not Supported
#endif

#define SK_PLATFORM_WINDOW_STATE \
  SK_WIN32_WINDOW_STATE
