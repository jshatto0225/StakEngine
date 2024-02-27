#if defined(SK_WINDOWS)

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"

LRESULT CALLBACK _sk_win32_message_callback(HWND window, u32 msg, WPARAM wparam, LPARAM lparam) {
  sk::WindowData *data = (sk::WindowData *)GetWindowLongPtrA(window, GWLP_USERDATA);
  if (data) {
    switch (msg) {
    case WM_CLOSE: {
      if (data->event_function) {
        sk::Event e = {};
        e.type = sk::WINDOW_CLOSE;
        e.win_close_event.window = data->window;
        data->event_function(e);
        return 0;
      }
      break;
    }
    case WM_SIZE: {
      RECT rect = {};
      GetClientRect(window, &rect);

      data->width = rect.right - rect.left;
      data->height = rect.bottom - rect.top;

      if (data->event_function) {
        sk::Event e;
        e.type = sk::WINDOW_RESIZED;
        e.win_resize_event.window = data->window;
        e.win_resize_event.height = rect.right - rect.left;
        e.win_resize_event.width = rect.bottom - rect.top;
        data->event_function(e);
        return 0;
      }
      break;
    }
    case WM_MOVE: {
      RECT rect = {};
      GetClientRect(window, &rect);
      data->x = rect.left;
      data->y = rect.top;

      if (data->event_function) {
        sk::Event e;
        e.type = sk::WINDOW_MOVED;
        e.win_move_event.window = data->window;
        e.win_move_event.x = rect.left;
        e.win_move_event.y = rect.top;
        data->event_function(e);
        return 0;
      }
      break;
    }
    }
  }
  return DefWindowProc(window, msg, wparam, lparam);
}

namespace sk {
bool Platform::initialized = false;
Win32Platform Platform::win32;

void Platform::init() {
  if (Platform::initialized) {
    return;
  }
  Platform::win32.instance = GetModuleHandle(nullptr);
  WNDCLASSEXA wnd_class = {};
  wnd_class.cbSize = sizeof(WNDCLASSEXA);
  wnd_class.lpszClassName = SK_WIN32_DEFAULT_WNDCLASS_NAME;
  wnd_class.hInstance = Platform::win32.instance;
  wnd_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
  wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wnd_class.lpfnWndProc = _sk_win32_message_callback;
  wnd_class.cbClsExtra = sizeof(Window *);

  Platform::win32.default_window_class = RegisterClassExA(&wnd_class);
  Platform::initialized = true;
}

bool Platform::is_initialized() {
  return Platform::initialized;
}

Proc Platform::get_proc_address(const char *name) {
  return (Proc)wglGetProcAddress(name);
}

u64 Window::count = 0;

Window::Window(const WindowConfig &config) {
  if (!Platform::is_initialized()) {
    Platform::init();
  }
  this->data.x = config.x;
  this->data.y = config.y;
  this->data.width = config.width;
  this->data.height = config.height;
  this->data.title = config.title;
  this->data.win32.instance = GetModuleHandle(nullptr);
  this->data.should_close = false;
  this->data.window = this;

  this->data.win32.handle = CreateWindowExA(0,
                                            SK_WIN32_DEFAULT_WNDCLASS_NAME,
                                            this->data.title,
                                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                            this->data.x,
                                            this->data.y,
                                            this->data.width,
                                            this->data.height,
                                            nullptr,
                                            nullptr,
                                            this->data.win32.instance,
                                            nullptr);
  SetWindowLongPtrA(this->data.win32.handle, GWLP_USERDATA, (LONG_PTR)this);
  this->context = new Context(this);
}

Window::~Window() {
  delete this->context;
  DestroyWindow(this->data.win32.handle);
}

void Window::set_pos(i32 x, i32 y) {
  SetWindowPos(this->data.win32.handle, nullptr, x, y, this->data.width, this->data.height, 0);
}

void Window::set_size(i32 width, i32 height) {
  SetWindowPos(this->data.win32.handle, nullptr, this->data.x, this->data.y, width, height, 0);
}

void Window::get_pos(i32 *x, i32 *y) {
  *x = this->data.x;
  *y = this->data.y;
}

void Window::get_size(i32 *width, i32 *height) {
  *width = this->data.width;
  *height = this->data.height;
}

void  Window::make_current() {
  this->context->make_current();
}

void Window::update() {
  MSG msg = {};
  PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE);
  TranslateMessage(&msg);
  DispatchMessage(&msg);

  this->context->swap_buffers();
}

void Window::set_event_callback(const std::function<void(Event &)> &func) {
  this->data.event_function = func;
}

namespace Input {
bool is_key_down(i32 key) {
  return GetAsyncKeyState(key);
}

void get_mouse_pos(i32 *x, i32 *y) {
  POINT pos = {};
  GetCursorPos(&pos);
  *x = pos.x;
  *y = pos.y;
}
} // namespace Input
} // namespace sk

#endif
