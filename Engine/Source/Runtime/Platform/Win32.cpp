#if defined(SK_WINDOWS)

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"

_SKPlatform _sk;

LRESULT CALLBACK _sk_win32_message_callback(HWND window, u32 msg, WPARAM wparam, LPARAM lparam) {
  _SKWindow *sk_win = (_SKWindow *)GetWindowLongPtrA(window, GWLP_USERDATA);
  if (sk_win) {
    switch (msg) {
    case WM_CLOSE:
    {
      sk_win->should_close = true;
      sk_win->callbacks.window_close_fun((SKWindow *)sk_win);
      return 0;
    }
    case WM_SIZE:
    {
      RECT rect = {};
      GetClientRect((HWND)sk_win->win32.handle, &rect);
      sk_win->width = rect.right - rect.bottom;
      sk_win->height = rect.bottom - rect.top;
      sk_win->callbacks.window_size_fun((SKWindow *)sk_win,
                                        rect.right - rect.left,
                                        rect.bottom - rect.top);
      return 0;
    }
    case WM_MOVE:
    {
      RECT rect = {};
      GetClientRect((HWND)sk_win->win32.handle, &rect);
      sk_win->x = rect.left;
      sk_win->y = rect.top;
      sk_win->callbacks.window_pos_fun((SKWindow *)sk_win, rect.left, rect.top);
      return 0;
    }
    }
  }
  return DefWindowProc(window, msg, wparam, lparam);
}

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
  wnd_class.lpfnWndProc = _sk_win32_message_callback;
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

SKWindow *sk_create_window(const SKWindowConfig &config) {
  _SKWindow *window = new _SKWindow;
  window->x = config.x;
  window->y = config.y;
  window->width = config.width;
  window->height = config.height;
  window->title = config.title;
  window->win32.instance = GetModuleHandle(nullptr);
  window->should_close = false;

  window->win32.handle = CreateWindowExA(0,
                                         SK_WIN32_DEFAULT_WNDCLASS_NAME,
                                         window->title,
                                         WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                         window->x,
                                         window->y,
                                         window->width,
                                         window->height,
                                         nullptr,
                                         nullptr,
                                         window->win32.instance,
                                         nullptr);
  SetWindowLongPtrA(window->win32.handle, GWLP_USERDATA, (LONG_PTR)window);
  _sk_create_context(window);

  // Add window to linked list
  if (_sk.window_head == nullptr) {
    _sk.window_head = window;
  }
  else {
    _sk.window_last->next = window;
  }
  _sk.window_last = window;

  return (SKWindow *)window;
}

void sk_destroy_window(SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  sk_make_context_current(nullptr);
  DestroyWindow(internal_window->win32.handle);

  // Remove window from list
  _SKWindow *curr = _sk.window_head;
  _SKWindow *prev = curr;
  while (curr != nullptr) {
    if (curr == internal_window) {
      prev->next = curr->next;
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  delete internal_window;
}

void sk_set_window_pos(SKWindow *win, i32 x, i32 y) {

}

void sk_set_window_size(SKWindow *win, i32 width, i32 height) {

}

void sk_get_window_pos(SKWindow *win, i32 *x, i32 *y) {

}

void sk_get_window_size(SKWindow *win, i32 *width, i32 *height) {

}

bool sk_window_should_close(SKWindow *win) {
  if (!win) {
    return false;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  return internal_window->should_close;
}

void sk_poll_events() {
  MSG msg = {};
  PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE);
  TranslateMessage(&msg);
  DispatchMessage(&msg);
}

void sk_set_window_close_callback(SKWindow *win, void(*func)(SKWindow *)) {
  if (!func || !win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->callbacks.window_close_fun = func;
}

void sk_set_window_size_callback(SKWindow *win, void(*func)(SKWindow *, i32, i32)) {
  if (!func || !win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->callbacks.window_size_fun = func;
}

void sk_set_window_pos_callback(SKWindow *win, void(*func)(SKWindow *, i32, i32)) {
  if (!func || !win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->callbacks.window_pos_fun = func;
}

#endif
