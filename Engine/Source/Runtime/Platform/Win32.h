#pragma once

#include <windows.h>

namespace sk {
struct Win32Platform {
  HINSTANCE instance;
  ATOM default_window_class;
};

struct Win32Window {
  HWND handle;
  HINSTANCE instance;
};
}

constexpr const char* SK_WIN32_DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";

#define WIN32_PLATFORM_STATE_DECLARATION Win32Platform Platform::win32;

#define WIN32_PLATFORM_STATE Win32Platform win32;
#define WIN32_WINDOW_STATE Win32Window win32;
