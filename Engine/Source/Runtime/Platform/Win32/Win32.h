#pragma once

#include <windows.h>

struct _SKWin32Platform {
  HINSTANCE instance = {};;
  ATOM default_window_class = {};
};

constexpr const char* SK_WIN32_DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";
#define SK_WIN32_PLATFORM_STATE _SKWin32Platform win32;

struct _SKWin32Window {
  HWND handle;
  HINSTANCE instance;
};

#define SK_WIN32_WINDOW_STATE _SKWin32Window win32;
