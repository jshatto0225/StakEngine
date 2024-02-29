#pragma once

#include <windows.h>

struct Win32Platform {
  HINSTANCE instance;
  ATOM default_window_class;

  HWND dummy_window;
};

struct Win32Window {
  HWND handle;
};

constexpr const char *SK_WIN32_DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";

#define WIN32_PLATFORM_STATE static Win32Platform win32;
#define WIN32_WINDOW_STATE Win32Window win32;
