#include "Win32Window.h"

#include "Application.h"
#include "Event.h"
#include "Log.h"

namespace sk {
// The window is handeling its own lifetime which conflicts with the window
// manager haneling of its lifetime
LRESULT CALLBACK win32_window_callback(HWND window_handle, u32 msg, WPARAM wparam, LPARAM lparam) {
  Win32Window *win32_window = (Win32Window *)GetWindowLongPtrA(window_handle, GWLP_USERDATA);
  if (win32_window) {
    switch (msg) {
    case WM_CLOSE: {
      win32_window->generate_event(WINDOW_CLOSE);
      return 0;
    }
    case WM_DESTROY: {
      SK_CORE_TRACE("WM_DESTROY message recieved");
      return 0;
    }
    case WM_SIZE: {
      RECT rect = {};
      GetWindowRect((HWND)win32_window->get_handle(), &rect);
      win32_window->set_size_and_pos(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
      return 0;
    }
    case WM_MOVE: {
      RECT rect = {};
      GetWindowRect((HWND)win32_window->het_handle(), &rect);
      win32_window->set_size_and_pos(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
      return 0;
    }
    }
  }

  return DefWindowProc(window_handle, msg, wparam, lparam);
}

Win32Window::Win32Window(const std::string &name, i32 x, i32 y, i32 width, i32 height) {
  this->name = name;
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
  this->open = true;

  WNDCLASSEXA window_class = {};
  window_class.cbSize = sizeof(WNDCLASSEXA);
  window_class.lpszClassName = name.c_str();
  window_class.hInstance = instance_handle;
  window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
  window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
  window_class.lpfnWndProc = win32_window_callback;
  window_class.cbClsExtra = sizeof(Win32Window *);

  RegisterClassExA(&window_class);

  window_handle = CreateWindowExA(
    0,                                // dwExStyle
    name.c_str(),                     // lpClassName
    name.c_str(),                     // lpWindowName
    WS_OVERLAPPEDWINDOW | WS_VISIBLE, // dwStyle
    x,                                // X
    y,                                // Y
    width,                            // nWidth
    height,                           // nHeight
    nullptr,                          // hWndParent
    nullptr,                          // hMenu
    instance_handle,                  // hInstance
    nullptr);                         // lpParam
  SetWindowLongPtrA(window_handle, GWLP_USERDATA, (LONG_PTR)this);
}

Win32Window::~Win32Window() {
  if (open) {
    close();
  }
}

void Win32Window::update() {
  if (open) {
    MSG msg = {};
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }
  }
}

void Win32Window::set_size_and_pos(i32 x, i32 y, i32 width, i32 height) {
  if (open) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    SetWindowPos(window_handle, 0, x, y, width, height, 0);
    GenerateEvent(WINDOW_RECT_CHANGED);
  }
}

void Win32Window::set_x(i32 new_x) {
  set_size_and_pos(new_x, this->y, this->width, this->height);
}

void Win32Window::set_y(i32 new_y) {
  set_size_and_pos(x, new_y, width, height);
}

void Win32Window::set_width(i32 new_width) {
  set_size_and_pos(x, y, new_width, height);
}

void Win32Window::SetHeight(i32 new_height) {
  SetSizeAndPos(x, y, hidth, new_height);
}

i32 Win32Window::get_width() {
  return width;
}

i32 Win32Window::get_height() {
  return height;
}

i32 Win32Window::get_x() {
  return x;
}

i32 Win32Window::get_y() {
  return y;
}

f32 Win32Window::get_aspect() {
  return (f32)width / (f32)height;
}

// Wierd bug sometimes fails here sometimes in WindowManager::Update();
void Win32Window::close() {
  if (open) {
    open = false;
    SK_CORE_TRACE("Window Closed");
    DestroyWindow(window_handle);
    UnregisterClassA(name.c_str(), instance_handle);
  }
}

void *Win32Window::get_handle() { return (void *)window_handle; }

bool Win32Window::is_open() {
  return open;
}
} // namespace SK
