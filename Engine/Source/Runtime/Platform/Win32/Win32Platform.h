#pragma once

#include <windows.h>

struct _SKWin32Platform {
  HINSTANCE instance = {};;
  ATOM default_window_class = {};
};

#define SK_WIN32_DEFAULT_WNDCLASS_NAME "SK_DEFAULT_WNDCLASS"
#define SK_WIN32_PLATFORM_STATE _SKWin32Platform win32;
