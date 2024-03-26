#include "Platform.h"

#if defined(SK_WINDOWS)

#include "Window.h"
#include "Log.h"
#include "Asserts.h"
#include "Input.h"
#include "Image.h"
#include "File.h"
#include "WindowsPlatform.h"

TRANSLATEMESSAGEPROC TranslateMessage;
DISPATCHMESSAGEAPROC DispatchMessageA;
PEEKMESSAGEAPROC PeekMessageA;
DEFWINDOWPROCAPROC DefWindowProcA;
UNREGISTERCLASSAPROC UnregisterClassA;
REGISTERCLASSEXAPROC RegisterClassExA;
CREATEWINDOWEXAPROC CreateWindowExA;
DESTROYWINDOWPROC Win32DestroyWindow;
SETWINDOWPOSPROC Win32SetWindowPos;
GETASYNCKEYSTATEPROC GetAsyncKeyState;
SETTIMERPROC SetTimer;
KILLTIMERPROC KillTimer;
BEGINPAINTPROC BeginPaint;
ENDPAINTPROC EndPaint;
REDRAWWINDOWPROC RedrawWindow;
GETCLIENTRECTPROC GetClientRect;
GETWINDOWLONGPTRAPROC GetWindowLongPtrA;
SETWINDOWLONGPTRAPROC SetWindowLongPtrA;
LOADCURSORWPROC LoadCursorW;
LOADICONWPROC LoadIconW;
CHOOSEPIXELFORMATPROC ChoosePixelFormat;
SETPIXELFORMATPROC SetPixelFormat;
SWAPBUFFERSPROC SwapBuffers;
GETDCPROC GetDC;

platform Platform;

constexpr const char *WIN32_DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";

const UINT TIMER_INTERVAL = 10;
UINT_PTR TimerId;

LRESULT CALLBACK Win32MessageCallback(HWND Window, u32 Msg, WPARAM WParam, LPARAM LParam) {
  window *Win = (window *)GetWindowLongPtrA(Window, GWLP_USERDATA);
  if (Win) {
    switch (Msg) {
    case WM_ENTERSIZEMOVE: {
      TimerId = SetTimer(Window, 1, TIMER_INTERVAL, NULL);
      return 0;
    };
    case WM_TIMER: {
      RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE);
      return 0;
    }
    case WM_EXITSIZEMOVE: {
      KillTimer(Window, TimerId);
      return 0;
    }
    case WM_CLOSE: {
      if (Win->EventFn) {
        event Event = {};
        Event.Type = WINDOW_CLOSE;
        Event.WinCloseEvent.Window = Win;
        Win->EventFn(Win->Parent, &Event);
      }
      return 0;
    }
    case WM_SIZE: {
      RECT Rect = {};
      GetClientRect(Window, &Rect);

      Win->Width = Rect.right - Rect.left;
      Win->Height = Rect.bottom - Rect.top;

      if (Win->EventFn) {
        event Event;
        Event.Type = WINDOW_RESIZED;
        Event.WinResizeEvent.Window = Win;
        Event.WinResizeEvent.Width = Rect.right - Rect.left;
        Event.WinResizeEvent.Height = Rect.bottom - Rect.top;
        Win->EventFn(Win->Parent, &Event);
      }
      RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE);
      return 0;
    }
    case WM_MOVE: {
      RECT Rect = {};
      GetClientRect(Window, &Rect);
      Win->X = Rect.left;
      Win->Y = Rect.top;

      if (Win->EventFn) {
        event Event;
        Event.Type = WINDOW_MOVED;
        Event.WinMoveEvent.Window = Win;
        Event.WinMoveEvent.X = Rect.left;
        Event.WinMoveEvent.Y = Rect.top;
        Win->EventFn(Win->Parent, &Event);
      }
      RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE);
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT PaintStruct;
      HDC DeviceContext = BeginPaint(Window, &PaintStruct);
      if (Win->EventFn) {
        event Event;
        Event.Type = WINDOW_PAINT;
        Event.WinPaintEvent.Window = Win;
        Win->EventFn(Win->Parent, &Event);
      }
      EndPaint(Window, &PaintStruct);
      return 0;
    }
    }
  }
  return DefWindowProcA(Window, Msg, WParam, LParam);
}

void LoadWin32Libs() {
  Platform.User32 = LoadLibraryA("User32.dll");
  Platform.Gdi32 = LoadLibraryA("Gdi32.dll");

  if (!Platform.User32) {
    LogCoreError("Failed to load User32");
    return;
  }
  if (!Platform.Gdi32) {
    LogCoreError("Failed to load Gdi32");
    return;
  }

  TranslateMessage = (TRANSLATEMESSAGEPROC)GetProcAddress(Platform.User32, "TranslateMessage");
  DispatchMessageA = (DISPATCHMESSAGEAPROC)GetProcAddress(Platform.User32, "DispatchMessageA");
  PeekMessageA = (PEEKMESSAGEAPROC)GetProcAddress(Platform.User32, "PeekMessageA");
  DefWindowProcA = (DEFWINDOWPROCAPROC)GetProcAddress(Platform.User32, "DefWindowProcA");
  UnregisterClassA = (UNREGISTERCLASSAPROC)GetProcAddress(Platform.User32, "UnregisterClassA");
  RegisterClassExA = (REGISTERCLASSEXAPROC)GetProcAddress(Platform.User32, "RegisterClassExA");
  CreateWindowExA = (CREATEWINDOWEXAPROC)GetProcAddress(Platform.User32, "CreateWindowExA");
  Win32DestroyWindow = (DESTROYWINDOWPROC)GetProcAddress(Platform.User32, "DestroyWindow");
  Win32SetWindowPos = (SETWINDOWPOSPROC)GetProcAddress(Platform.User32, "SetWindowPos");
  GetAsyncKeyState = (GETASYNCKEYSTATEPROC)GetProcAddress(Platform.User32, "GetAsyncKeyState");
  SetTimer = (SETTIMERPROC)GetProcAddress(Platform.User32, "SetTimer");
  KillTimer = (KILLTIMERPROC)GetProcAddress(Platform.User32, "KillTimer");
  BeginPaint = (BEGINPAINTPROC)GetProcAddress(Platform.User32, "BeginPaint");
  EndPaint = (ENDPAINTPROC)GetProcAddress(Platform.User32, "EndPaint");
  RedrawWindow = (REDRAWWINDOWPROC)GetProcAddress(Platform.User32, "RedrawWindow");
  GetClientRect = (GETCLIENTRECTPROC)GetProcAddress(Platform.User32, "GetClientRect");
  GetWindowLongPtrA = (GETWINDOWLONGPTRAPROC)GetProcAddress(Platform.User32, "GetWindowLongPtrA");
  SetWindowLongPtrA = (SETWINDOWLONGPTRAPROC)GetProcAddress(Platform.User32, "SetWindowLongPtrA");
  LoadCursorW = (LOADCURSORWPROC)GetProcAddress(Platform.User32, "LoadCursorW");
  LoadIconW = (LOADICONWPROC)GetProcAddress(Platform.User32, "LoadIconW");
  GetDC = (GETDCPROC)GetProcAddress(Platform.User32, "GetDC");

  ChoosePixelFormat = (CHOOSEPIXELFORMATPROC)GetProcAddress(Platform.Gdi32, "ChoosePixelFormat");
  SetPixelFormat = (SETPIXELFORMATPROC)GetProcAddress(Platform.Gdi32, "SetPixelFormat");
  SwapBuffers = (SWAPBUFFERSPROC)GetProcAddress(Platform.Gdi32, "SwapBuffers");
}

void PlatformInit() {
  if (Platform.Initialized) {
    return;
  }

  LoadWin32Libs();

  Platform.Instance = GetModuleHandleW(NULL);

  WNDCLASSEXA WindowClass = {};
  WindowClass.cbSize = sizeof(WNDCLASSEXA);
  WindowClass.lpszClassName = WIN32_DEFAULT_WNDCLASS_NAME;
  WindowClass.hInstance = Platform.Instance;
  WindowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  WindowClass.lpfnWndProc = Win32MessageCallback;
  WindowClass.cbClsExtra = sizeof(window *);
  Platform.DefaultWindowClass = RegisterClassExA(&WindowClass);

  Platform.DummyWindow = CreateWindowExA(0,
                                         WIN32_DEFAULT_WNDCLASS_NAME,
                                         "Dummy Window",
                                         0,
                                         0,
                                         0,
                                         0,
                                         0,
                                         NULL,
                                         NULL,
                                         Platform.Instance,
                                         NULL);

  PlatformInitExtensions();

  Platform.Initialized = true;
}

void PlatformShutdown() {
  if (!Platform.Initialized) {
    return;
  }
  Platform.Initialized = false;
  UnregisterClassA(WIN32_DEFAULT_WNDCLASS_NAME, Platform.Instance);
}

bool PlatformIsInitialized() {
  return Platform.Initialized;
}

u64 WindowCount = 0;

window *CreateWindow(const window_config *Config) {
  if (WindowCount == 0) {
    PlatformInit();
  }
  WindowCount++;

  window *Window = (window *)malloc(sizeof(window));

  if (!Window) {
    LogCoreError("Failed to allocate memory for window");
    return NULL;
  }
  
  Window->Parent = Config->Parent;
  Window->X = Config->X;
  Window->Y = Config->Y;
  Window->Width = Config->Width;
  Window->Height = Config->Height;
  Window->Title = Config->Title;

  Window->Handle = CreateWindowExA(0,
                                   WIN32_DEFAULT_WNDCLASS_NAME,
                                   Window->Title,
                                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                   Window->X,
                                   Window->Y,
                                   Window->Width,
                                   Window->Height,
                                   NULL,
                                   NULL,
                                   Platform.Instance,
                                   NULL);
  SetWindowLongPtrA(Window->Handle, GWLP_USERDATA, (LONG_PTR)Window);
  Window->Context = CreateContext(Window);

  return Window;
}

void DestroyWindow(window **Window) {
  if (*Window) {
    DestroyContext(&(*Window)->Context);
    
    Win32DestroyWindow((*Window)->Handle);
    WindowCount--;
    if (WindowCount == 0) {
      PlatformShutdown();
    }

    free(*Window);

    *Window = NULL;
  }
}

void SetWindowPos(window *Window, i32 X, i32 Y) {
  Win32SetWindowPos(Window->Handle, NULL, X, Y, Window->Width, Window->Height, 0);
  Window->X = X;
  Window->Y = Y;
}

void SetWindowSize(window *Window, i32 Width, i32 Height) {
  Win32SetWindowPos(Window->Handle, NULL, Window->X, Window->Y, Width, Height, 0);
  Window->Width = Width;
  Window->Height = Height;
}

window_pos_data GetWindowPos(window *Window) {
  return { Window->X, Window->Y };
}

window_size_data GetWindowSize(window *Window) {
  return { Window->Width, Window->Height };
}

void MakeWindowCurrent(const window *Window) {
  MakeContextCurrent(Window->Context);
}

void UpdateWindow(const window *Window) {
  RedrawWindow(Window->Handle, NULL, NULL, RDW_INVALIDATE);
  MSG Msg = {};
  while (PeekMessageA(&Msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&Msg);
    DispatchMessageA(&Msg);
  }
}

void SwapWindowBuffers(window *Window) {
  SwapContextBuffers(Window->Context);
}

void SetWindowEventFn(window *Window, void(*EventFn)(void *, const event *)) {
  Window->EventFn = EventFn;
}

const i32 KeyMap[K_LAST + 1] = {
  VK_LBUTTON,
  VK_RBUTTON,
  VK_MBUTTON,
  VK_SPACE,
  VK_OEM_7,
  VK_OEM_COMMA,
  VK_OEM_MINUS,
  VK_OEM_PERIOD,
  VK_OEM_2,
  0x30,
  0x31,
  0x32,
  0x33,
  0x34,
  0x35,
  0x36,
  0x37,
  0x38,
  0x39,
  VK_OEM_1,
  VK_OEM_PLUS,
  0x41,
  0x42,
  0x43,
  0x44,
  0x45,
  0x46,
  0x47,
  0x48,
  0x49,
  0x4A,
  0x4B,
  0x4C,
  0x4D,
  0x4E,
  0x4F,
  0x50,
  0x51,
  0x52,
  0x53,
  0x54,
  0x55,
  0x56,
  0x57,
  0x58,
  0x59,
  0x5A,
  VK_OEM_4,
  VK_OEM_4,
  VK_OEM_6,
  VK_OEM_3,
  VK_ESCAPE,
  VK_RETURN,
  VK_TAB,
  VK_BACK,
  VK_INSERT,
  VK_DELETE,
  VK_RIGHT,
  VK_LEFT,
  VK_DOWN,
  VK_UP,
  VK_PRIOR,
  VK_NEXT,
  VK_HOME,
  VK_END,
  VK_CAPITAL,
  VK_SCROLL,
  VK_NUMLOCK,
  VK_SNAPSHOT,
  VK_PAUSE,
  VK_F1,
  VK_F2,
  VK_F3,
  VK_F4,
  VK_F5,
  VK_F6,
  VK_F7,
  VK_F8,
  VK_F9,
  VK_F10,
  VK_F11,
  VK_F12,
  VK_F13,
  VK_F14,
  VK_F15,
  VK_F16,
  VK_F17,
  VK_F18,
  VK_F19,
  VK_F20,
  VK_F21,
  VK_F22,
  VK_F23,
  VK_F24,
  VK_NUMPAD0,
  VK_NUMPAD1,
  VK_NUMPAD2,
  VK_NUMPAD3,
  VK_NUMPAD4,
  VK_NUMPAD5,
  VK_NUMPAD6,
  VK_NUMPAD7,
  VK_NUMPAD8,
  VK_NUMPAD9,
  VK_DECIMAL,
  VK_DIVIDE,
  VK_MULTIPLY,
  VK_SUBTRACT,
  VK_ADD,
  VK_RETURN,
  VK_RETURN, // NOTE: On Windows this key does not exist
  VK_LSHIFT,
  VK_LCONTROL,
  VK_LMENU,
  VK_LWIN,
  VK_RSHIFT,
  VK_RCONTROL,
  VK_RMENU,
  VK_RWIN,
  VK_MENU
};

bool GetKeyDown(key Key) {
  return GetAsyncKeyState(KeyMap[Key]);
}

vec2 GetMousePos() {
  POINT Pos = {};
  GetCursorPos(&Pos);
  return vec2{ (f32)Pos.x, (f32)Pos.y };
}

void LPOverlappedCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
  // TODO: IDEK
}

file *CreateFile(const char *Path) {
  file *File = (file *)malloc(sizeof(file));

  if (!File) {
    LogCoreError("Failed to allocate memory for file");
    return NULL;
  }
  
  File->FilePath = Path;
  File->Data = NULL;
  HANDLE FileHandle = CreateFileA(File->FilePath,
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  0,
                                  OPEN_EXISTING,
                                  0,
                                  0);
  if (!FileHandle) {
    // Error
    LogCoreError("Failed to open file");
    return NULL;
  }
  LARGE_INTEGER FileSize;
  if (!GetFileSizeEx(FileHandle, &FileSize)) {
    // Error
    LogCoreError("File size error");
    CloseHandle(FileHandle);
    return NULL;
  }
  File->Size = FileSize.QuadPart;
  File->Data = VirtualAlloc(NULL, FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (!File->Data) {
    // Error
    LogCoreError("Could not allocate password");
    CloseHandle(FileHandle);
    return NULL;
  }
  OVERLAPPED IdkWhatThisDoes = {};
  if (!ReadFileEx(FileHandle, File->Data, FileSize.QuadPart, &IdkWhatThisDoes, LPOverlappedCompletionRoutine)) {
    LogCoreError("Could not read file");
    VirtualFree(File->Data, File->Size, MEM_RESERVE);
    CloseHandle(FileHandle);
    File->Data = NULL;
    return NULL;
  }
  CloseHandle(FileHandle);

  return File;
}

void DestroyFile(file **File) {
  if (*File) {
    VirtualFree((*File)->Data, (*File)->Size, MEM_RESERVE);

    free(*File);

    *File = NULL;
  }
}

bool IsBMPFile(const char *Path) {
  int Len = strlen(Path);
  if (Len < 4) // If string length is less than 4, it can't end with ".bmp"
    return 0;

  // Compare the last four characters with ".bmp"
  return strcmp(Path + Len - 4, ".bmp") == 0;
}

image *CreateImage(const char *Path) {
  image *Image = (image *)malloc(sizeof(image));

  if (!Image) {
    LogCoreError("Failed to allocate memory for image");
    return NULL;
  }

  Image->Data.Path = Path;
  Image->Bytes = NULL;

  file *File = CreateFile(Image->Data.Path);
  u8 *FileData = (u8 *)File->Data;
  u64 FileSize = File->Size;
  if (IsBMPFile(Path)) {
    LoadImageAsBMP(Image, FileData, FileSize);
    DestroyFile(&File);
    return Image;
  }
  LogCoreError("Unsupported Image Format");
  return NULL;
}

void DestroyImage(image **Image) {
  if (*Image) {
    VirtualFree((*Image)->Bytes, (*Image)->Data.SizeInBytes, MEM_RESERVE);

    free(*Image);

    *Image = NULL;
  }
}

void LoadImageAsBMP(image *Image, u8 *FileData, u64 FileSize) {
  if (FileData[0] != 'B' || FileData[1] != 'M') {
    LogCoreError("Did not find BM");
    return;
  }
  Image->Data.SizeInBytes = *((u32 *)(FileData + 2));
  u32 Offset = *((u32 *)(FileData + 10));
  Image->Data.Width = *((i32 *)(FileData + 18));
  Image->Data.Height = *((i32 *)(FileData + 22));
  u16 BPP = *((u16 *)(FileData + 28));
  switch (BPP) {
  case 1:
    Image->Data.Channels = 1;
    break;
  case 8:
    Image->Data.Channels = 1;
    break;
  case 24:
    Image->Data.Channels = 3;
    break;
  case 32:
    Image->Data.Channels = 4;
    break;
  default:
    LogCoreError("Invalid BPP: %d", BPP);
    Image->Data.Channels = 0;
    break;
  }
  Image->Bytes = (u8 *)VirtualAlloc(NULL,
                                    Image->Data.SizeInBytes,
                                    MEM_RESERVE | MEM_COMMIT,
                                    PAGE_READWRITE);
  if (!Image->Bytes) {
    LogCoreError("Allocation Failed");
    return;
  }
  memcpy(Image->Bytes, FileData + Offset, Image->Data.SizeInBytes);
}

#endif
