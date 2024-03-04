#if defined(SK_WINDOWS)

#include <map>

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"
#include "Log.h"
#include "Input.h"
#include "StakMath.h"
#include "Image.h"

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

void Window::setEventCallback(const EventFn &func) {
  this->data.event_function = func;
}

const std::map<Key, i32> KeyMap = {
  {Key::BUTTON_1, 0x01},
  {Key::BUTTON_2, 0x02},
  {Key::BUTTON_3, 0x03},
  {Key::SPACE, 0x20},
  {Key::APOSTROPHE, 0x0},
  {Key::COMMA, 0xBC},
  {Key::MINUS, 0xBD},
  {Key::PERIOD, 0xBE},
  {Key::SLASH, 0xBF},
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
  {Key::SEMICOLON, 0xBA},
  {Key::EQUAL, 0x0},
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
  {Key::LEFT_BRACKET, 0xBD},
  {Key::BACKSLASH, 0xDC},
  {Key::RIGHT_BRACKET, 0xDD},
  {Key::GRAVE, 0x0},
  {Key::ESCAPE, 0x0},
  {Key::ENTER, 0x0},
  {Key::TAB, 0x0},
  {Key::BACKSPACE, 0x08},
  {Key::INSTERT, 0x2D},
  {Key::DELETE, 0x2E},
  {Key::RIGHT, 0x27},
  {Key::LEFT, 0x25},
  {Key::DOWN, 0x28},
  {Key::UP, 0x26},
  {Key::PAGE_UP, 0x21},
  {Key::PAGE_DOWN, 0x22},
  {Key::HOME, 0x24},
  {Key::END, 0x23},
  {Key::CAPS_LOCK, 0x0},
  {Key::SCROLL_LOCK, 0x91},
  {Key::NUM_LOCK, 0x90},
  {Key::PRINT_SCREEN, 0x2C},
  {Key::PAUSE, 0x0},
  {Key::F1, 0x70},
  {Key::F2, 0x71},
  {Key::F3, 0x72},
  {Key::F4, 0x73},
  {Key::F5, 0x74},
  {Key::F6, 0x75},
  {Key::F7, 0x76},
  {Key::F8, 0x77},
  {Key::F9, 0x78},
  {Key::F10, 0x79},
  {Key::F11, 0x7A},
  {Key::F12, 0x7B},
  {Key::F13, 0x7C},
  {Key::F14, 0x7D},
  {Key::F15, 0x7E},
  {Key::F16, 0x7F},
  {Key::F17, 0x80},
  {Key::F18, 0x81},
  {Key::F19, 0x82},
  {Key::F20, 0x83},
  {Key::F21, 0x84},
  {Key::F22, 0x85},
  {Key::F23, 0x86},
  {Key::F24, 0x87},
  {Key::F25, 0x88},
  {Key::NUM_ZERO, 0x60},
  {Key::NUM_ONE, 0x61},
  {Key::NUM_TWO, 0x62},
  {Key::NUM_THREE, 0x63},
  {Key::NUM_FOUR, 0x64},
  {Key::NUM_FIVE, 0x65},
  {Key::NUM_SIX, 0x66},
  {Key::NUM_SEVEN, 0x67},
  {Key::NUM_EIGHT, 0x68},
  {Key::NUM_NINE, 0x69},
  {Key::NUM_DECIMAL, 0x6E},
  {Key::NUM_DIVIDE, 0x66F},
  {Key::NUM_MULTIPLY, 0x6A},
  {Key::NUM_SUBTRACT, 0x6D},
  {Key::NUM_ADD, 0x0},
  {Key::NUM_ENTER, 0x0},
  {Key::NUM_EQUAL, 0x0},
  {Key::LEFT_SHIFT, 0xA0},
  {Key::LEFT_CONTROL, 0xA2},
  {Key::LEFT_ALT, 0x0},
  {Key::LEFT_SUPER, 0x0},
  {Key::RIGHT_SHIFT, 0xA1},
  {Key::RIGHT_CONTROL, 0xA3},
  {Key::RIGHT_ALT, 0x0},
  {Key::RIGHT_SUPER, 0x0},
  {Key::MENU, 0x0},
};

bool Input::GetKeyDown(Key key) {
  return GetAsyncKeyState(KeyMap.at(key));
}

Vec2 Input::GetMousePos() {
  POINT pos = {};
  GetCursorPos(&pos);
  return Vec2(pos.x, pos.y);
}

constexpr int BMP_HEADER_SIZE = 54;

u8 *Image::LoadBMP(FILE *f, i32 *width, i32 *height, i32 *channels_in_image, i32 desired_channels) {
  if (!f) {
    Log::CoreError("Failed to load file");
    return nullptr;
  }

  // Read BMP header
  fseek(f, 0, SEEK_SET);
  char header[BMP_HEADER_SIZE];
  if (fread(header, 1, BMP_HEADER_SIZE, f) != BMP_HEADER_SIZE) {
    Log::CoreError("Failed to read BMP header");
    return nullptr;
  }

  // Verify BMP magic number
  if (header[0] != 'B' || header[1] != 'M') {
    Log::CoreError("Invalid BMP format");
    return nullptr;
  }

  // Extract width, height, and bit depth from BMP header
  *width = *reinterpret_cast<int *>(&header[18]);
  *height = *reinterpret_cast<int *>(&header[22]);
  u16 bit_depth = *reinterpret_cast<u16 *>(&header[28]);

  // Ensure bit depth is 32 bits (8 bits per channel)
  if (bit_depth != 32) {
    Log::CoreWarn("Bit Depth: %d", bit_depth);
    return nullptr;
  }

  // Move to the beginning of image data
  fseek(f, *reinterpret_cast<int *>(&header[10]), SEEK_SET);

  // Calculate pixel count and allocate memory for pixel data
  u32 pixel_count = *width * *height;
  u8 *pixels = new u8[pixel_count * desired_channels];
  if (!pixels) {
    Log::CoreError("Failed to allocate memory for pixel data");
    return nullptr;
  }

  // Read pixel data
  u32 actual_count = fread(pixels, desired_channels, pixel_count, f);
  if (actual_count != pixel_count) {
    Log::CoreError("Failed to read pixel data");
    delete[] pixels;
    return nullptr;
  }

  // Set the number of channels
  *channels_in_image = desired_channels;

  return pixels;
}

#endif
