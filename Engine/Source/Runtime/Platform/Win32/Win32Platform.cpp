#include "Platform.h"
#include "Window.h"

_SKPlatform _sk;

LRESULT CALLBACK Win32WindowCallback(HWND, u32, WPARAM, LPARAM);

void sk_platform_init() {
  if (_sk.initialized) {
    return;
  }
  _sk.win32.instance = GetModuleHandle(nullptr);
  WNDCLASSEXA wnd_class = {};
  wnd_class.cbSize = sizeof(WNDCLASSEXA);
  wnd_class.lpszClassName = SK_WIN32_DEFAULT_WNDCLASS_NAME;
  wnd_class.hInstance = _sk.win32.instance;
  wnd_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
  wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wnd_class.lpfnWndProc = Win32WindowCallback;
  wnd_class.cbClsExtra = sizeof(_SKWindow *);

  _sk.win32.default_window_class = RegisterClassExA(&wnd_class);
}

void sk_platform_shutdown() {
  if (!_sk.initialized) {
    return;
  }
  UnregisterClassA(MAKEINTATOM(_sk.win32.default_window_class), _sk.win32.instance);
  _sk = {};
}
