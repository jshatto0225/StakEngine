#pragma once

#include <windows.h>

struct _SKWin32Window {
  HWND handle;
  HINSTANCE instance;
};

#define SK_WIN32_WINDOW_STATE _SKWin32Window win32;
