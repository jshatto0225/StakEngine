#if defined(SK_WINDOWS)

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"
#include "Log.h"
#include "Input.h"
#include "Math.h"

LRESULT CALLBACK Win32MessageCallback(HWND window, u32 msg, WPARAM wparam, LPARAM lparam) {
  WindowData *data = (WindowData *)GetWindowLongPtrA(window, GWLP_USERDATA);
  if (data) {
    switch (msg) {
    case WM_CLOSE: {
      if (data->event_function) {
        Event e = {};
        e.type = WINDOW_CLOSE;
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
        Event e;
        e.type = WINDOW_RESIZED;
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
        Event e;
        e.type = WINDOW_MOVED;
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

bool Platform::initialized = false;
Win32Platform Platform::win32;

void Platform::Init() {
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
  wnd_class.lpfnWndProc = Win32MessageCallback;
  wnd_class.cbClsExtra = sizeof(Window *);

  Platform::win32.default_window_class = RegisterClassExA(&wnd_class);
  Platform::initialized = true;
}

void Platform::Shutdown() {
  if (!Platform::initialized) {
    return;
  }
  Platform::initialized = false;
  UnregisterClassA(SK_WIN32_DEFAULT_WNDCLASS_NAME, Platform::win32.instance);
}

bool Platform::IsInitialized() {
  return Platform::initialized;
}

Proc Platform::GetProcAddress(const char *name) {
  return (Proc)wglGetProcAddress(name);
}

u64 Window::count = 0;

Window::Window(const WindowConfig &config) {
  if (Window::count == 0) {
    Platform::Init();
  }
  Window::count++;

  this->data.x = config.x;
  this->data.y = config.y;
  this->data.width = config.width;
  this->data.height = config.height;
  this->data.title = config.title;
  this->data.win32.instance = GetModuleHandle(nullptr);
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
  SetWindowLongPtrA(this->data.win32.handle, GWLP_USERDATA, (LONG_PTR)&this->data);
  this->context = std::make_unique<Context>(this);
}

Window::~Window() {
  DestroyWindow(this->data.win32.handle);
  Window::count--;
  if (Window::count == 0) {
    Platform::Shutdown();
  }
}

void Window::setPos(i32 x, i32 y) {
  SetWindowPos(this->data.win32.handle, nullptr, x, y, this->data.width, this->data.height, 0);
  this->data.x = x;
  this->data.y = y;
}

void Window::setSize(i32 width, i32 height) {
  SetWindowPos(this->data.win32.handle, nullptr, this->data.x, this->data.y, width, height, 0);
  this->data.width = width;
  this->data.height = height;
}

Vec2 Window::getPos() {
  return Vec2(this->data.x, this->data.y);
}

Vec2 Window::getSize() {
  return Vec2(this->data.width, this->data.height);
}

void  Window::makeCurrent() {
  this->context->makeCurrent();
}

void Window::update() {
  MSG msg = {};
  PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE);
  TranslateMessage(&msg);
  DispatchMessage(&msg);

  this->context->swapBuffers();
}

void Window::setEventCallback(const std::function<void(Event &)> &func) {
  this->data.event_function = func;
}

bool Input::GetKeyDown(i32 key) {
  return GetAsyncKeyState(key);
}

Vec2 Input::GetMousePos() {
  POINT pos = {};
  GetCursorPos(&pos);
  return Vec2(pos.x, pos.y);
}

#endif
