#if defined(SK_WINDOWS)

#include <map>

#include "Platform.h"
#include "Window.h"
#include "Log.h"
#include "Input.h"
#include "Image.h"
#include "File.h"

namespace sk {
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
        e.win_resize_event.width = rect.right - rect.left;
        e.win_resize_event.height = rect.bottom - rect.top;
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

  Platform::win32.dummy_window = CreateWindowExA(0,
                                                 SK_WIN32_DEFAULT_WNDCLASS_NAME,
                                                 "Dummy Window",
                                                 0,
                                                 0,
                                                 0,
                                                 0,
                                                 0,
                                                 nullptr,
                                                 nullptr,
                                                 Platform::win32.instance,
                                                 nullptr);

  Platform::InitExtensions();

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
                                            Platform::win32.instance,
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
  return Vec2{ (f32)this->data.x, (f32)this->data.y };
}

Vec2 Window::getSize() {
  return Vec2{ (f32)this->data.width, (f32)this->data.height };
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

void Window::setEventCallback(const EventFn &func) {
  this->data.event_function = func;
}

// TODO: Finish
const std::map<Key, i32> KeyMap = {
  {Key::BUTTON_1, VK_LBUTTON},
  {Key::BUTTON_2, VK_RBUTTON},
  {Key::BUTTON_3, VK_MBUTTON},
  {Key::SPACE, VK_SPACE},
  {Key::APOSTROPHE, VK_OEM_7},
  {Key::COMMA, VK_OEM_COMMA},
  {Key::MINUS, VK_OEM_MINUS},
  {Key::PERIOD, VK_OEM_PERIOD},
  {Key::SLASH, VK_OEM_2},
  {Key::ZERO, 0x30},
  {Key::ONE, 0x31},
  {Key::TWO, 0x32},
  {Key::THREE, 0x33},
  {Key::FOUR, 0x34},
  {Key::FIVE, 0x35},
  {Key::SIX, 0x36},
  {Key::SEVEN, 0x37},
  {Key::EIGHT, 0x38},
  {Key::NINE, 0x39},
  {Key::SEMICOLON, VK_OEM_1},
  {Key::EQUAL, VK_OEM_PLUS},
  {Key::A, 0x41},
  {Key::B, 0x42},
  {Key::C, 0x43},
  {Key::D, 0x44},
  {Key::E, 0x45},
  {Key::F, 0x46},
  {Key::G, 0x47},
  {Key::H, 0x48},
  {Key::I, 0x49},
  {Key::J, 0x4A},
  {Key::K, 0x4B},
  {Key::L, 0x4C},
  {Key::M, 0x4D},
  {Key::N, 0x4E},
  {Key::O, 0x4F},
  {Key::P, 0x50},
  {Key::Q, 0x51},
  {Key::R, 0x52},
  {Key::S, 0x53},
  {Key::T, 0x54},
  {Key::U, 0x55},
  {Key::V, 0x56},
  {Key::W, 0x57},
  {Key::X, 0x58},
  {Key::Y, 0x59},
  {Key::Z, 0x5A},
  {Key::LEFT_BRACKET, VK_OEM_4},
  {Key::BACKSLASH, VK_OEM_4},
  {Key::RIGHT_BRACKET, VK_OEM_6},
  {Key::GRAVE, VK_OEM_3},
  {Key::ESCAPE, VK_ESCAPE},
  {Key::ENTER, VK_RETURN},
  {Key::TAB, VK_TAB},
  {Key::BACKSPACE, VK_BACK},
  {Key::INSTERT, VK_INSERT},
  {Key::DELETE, VK_DELETE},
  {Key::RIGHT, VK_RIGHT},
  {Key::LEFT, VK_LEFT},
  {Key::DOWN, VK_DOWN},
  {Key::UP, VK_UP},
  {Key::PAGE_UP, VK_PRIOR},
  {Key::PAGE_DOWN, VK_NEXT},
  {Key::HOME, VK_HOME},
  {Key::END, VK_END},
  {Key::CAPS_LOCK, VK_CAPITAL},
  {Key::SCROLL_LOCK, VK_SCROLL},
  {Key::NUM_LOCK, VK_NUMLOCK},
  {Key::PRINT_SCREEN, VK_SNAPSHOT},
  {Key::PAUSE, VK_PAUSE},
  {Key::F1, VK_F1},
  {Key::F2, VK_F2},
  {Key::F3, VK_F3},
  {Key::F4, VK_F4},
  {Key::F5, VK_F5},
  {Key::F6, VK_F6},
  {Key::F7, VK_F7},
  {Key::F8, VK_F8},
  {Key::F9, VK_F9},
  {Key::F10, VK_F10},
  {Key::F11, VK_F11},
  {Key::F12, VK_F12},
  {Key::F13, VK_F13},
  {Key::F14, VK_F14},
  {Key::F15, VK_F15},
  {Key::F16, VK_F16},
  {Key::F17, VK_F17},
  {Key::F18, VK_F18},
  {Key::F19, VK_F19},
  {Key::F20, VK_F20},
  {Key::F21, VK_F21},
  {Key::F22, VK_F22},
  {Key::F23, VK_F23},
  {Key::F24, VK_F24},
  {Key::NUM_ZERO, VK_NUMPAD0},
  {Key::NUM_ONE, VK_NUMPAD1},
  {Key::NUM_TWO, VK_NUMPAD2},
  {Key::NUM_THREE, VK_NUMPAD3},
  {Key::NUM_FOUR, VK_NUMPAD4},
  {Key::NUM_FIVE, VK_NUMPAD5},
  {Key::NUM_SIX, VK_NUMPAD6},
  {Key::NUM_SEVEN, VK_NUMPAD7},
  {Key::NUM_EIGHT, VK_NUMPAD8},
  {Key::NUM_NINE, VK_NUMPAD9},
  {Key::NUM_DECIMAL, VK_DECIMAL},
  {Key::NUM_DIVIDE, VK_DIVIDE},
  {Key::NUM_MULTIPLY, VK_MULTIPLY},
  {Key::NUM_SUBTRACT, VK_SUBTRACT},
  {Key::NUM_ADD, VK_ADD},
  {Key::NUM_ENTER, VK_RETURN},
  {Key::NUM_EQUAL, VK_RETURN}, // NOTE: On Windows this key does not exist
  {Key::LEFT_SHIFT, VK_LSHIFT},
  {Key::LEFT_CONTROL, VK_LCONTROL},
  {Key::LEFT_ALT, VK_LMENU},
  {Key::LEFT_SUPER, VK_LWIN},
  {Key::RIGHT_SHIFT, VK_RSHIFT},
  {Key::RIGHT_CONTROL, VK_RCONTROL},
  {Key::RIGHT_ALT, VK_RMENU},
  {Key::RIGHT_SUPER, VK_RWIN},
  {Key::MENU, VK_MENU},
};

bool Input::GetKeyDown(Key key) {
  return GetAsyncKeyState(KeyMap.at(key));
}

Vec2 Input::GetMousePos() {
  POINT pos = {};
  GetCursorPos(&pos);
  return Vec2{ (f32)pos.x, (f32)pos.y };
}

File::File(const std::string &path) {
  this->path = path.c_str();
  this->data = nullptr;
  HANDLE file_handle = CreateFileA(this->path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  if (!file_handle) {
    // Error
    Log::CoreError("Failed to open file");
    return;
  }
  LARGE_INTEGER file_size;
  if (!GetFileSizeEx(file_handle, &file_size)) {
    // Error
    Log::CoreError("File size error");
    CloseHandle(file_handle);
    return;
  }
  this->size = file_size.QuadPart;
  this->data = VirtualAlloc(nullptr, file_size.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (!this->data) {
    // Error
    Log::CoreError("Could not allocate password");
    CloseHandle(file_handle);
    return;
  }
  OVERLAPPED idk_what_this_does = {};
  if (!ReadFileEx(file_handle, this->data, file_size.QuadPart, &idk_what_this_does, nullptr)) {
    Log::CoreError("Could not read file");
    VirtualFree(this->data, this->size, MEM_RESERVE);
    CloseHandle(file_handle);
    this->data = nullptr;
    return;
  }
  CloseHandle(file_handle);
}

void *File::getData() {
  return this->data;
}

u64 File::getSize() {
  return this->size;
}

File::~File() {
  if (this->data) {
    VirtualFree(this->data, this->size, MEM_RESERVE);
  }
}

Image::Image(const std::string &path) {
  this->data.path = path;
  this->bytes = nullptr;

  File f(this->data.path);
  u8 *file_data = (u8 *)f.getData();
  u64 file_size = f.getSize();
  if (path.find(".bmp") != std::string::npos) {
    LoadAsBMP(file_data, file_size);
    return;
  }
  Log::CoreError("Unsupported Image Format");
}

Image::~Image() {
  if (this->bytes) {
    VirtualFree(this->bytes, this->data.size_in_bytes, MEM_RESERVE);
  }
}

void Image::LoadAsBMP(u8 *file_data, u64 file_size) {
  if (file_data[0] != 'B' || file_data[1] != 'M') {
    Log::CoreError("Did not find BM");
    return;
  }
  this->data.size_in_bytes = *((u32 *)(file_data + 2));
  u32 offset = *((u32 *)(file_data + 10));
  this->data.width = *((i32 *)(file_data + 18));
  this->data.height = *((i32 *)(file_data + 22));
  u16 bpp = *((u16 *)(file_data + 28));
  switch (bpp) {
  case 1:
    this->data.channels = 1;
    break;
  case 8:
    this->data.channels = 1;
    break;
  case 24:
    this->data.channels = 24;
    break;
  case 32:
    this->data.channels = 32;
    break;
  default:
    Log::CoreError("Invalid BPP: %d", bpp);
    this->data.channels = 0;
    break;
  }
  this->bytes = (u8 *)VirtualAlloc(nullptr, this->data.size_in_bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (!this->bytes) {
    Log::CoreError("Allocation Failed");
    return;
  }
  CopyMemory(this->bytes, file_data + offset, this->data.size_in_bytes);
}

u8 *Image::getBytes() {
  return this->bytes;
}

const ImageData &Image::getData() {
  return this->data;
}

} // namespace sk

#endif
