#include "Platform.h"

#ifdef SK_WINDOWS

#include "Window.h"
#include "Log.h"
#include "Asserts.h"
#include "Input.h"
#include "Image.h"
#include "File.h"
#include "Win32Platform.h"

/*********************
 * Private Interface *
 *********************/

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
GETCURSORPOSPROC GetCursorPos;

platform Platform;

void
LoadWin32Libs()
{
  Platform.User32 = LoadLibraryA("User32.dll");
  Platform.Gdi32 = LoadLibraryA("Gdi32.dll");

  if (!Platform.User32)
  {
    LogCoreError("Failed to load User32");
    return;
  }
  if (!Platform.Gdi32)
  {
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
  GetCursorPos = (GETCURSORPOSPROC)GetProcAddress(Platform.User32, "GetCursorPos");

  ChoosePixelFormat = (CHOOSEPIXELFORMATPROC)GetProcAddress(Platform.Gdi32, "ChoosePixelFormat");
  SetPixelFormat = (SETPIXELFORMATPROC)GetProcAddress(Platform.Gdi32, "SetPixelFormat");
  SwapBuffers = (SWAPBUFFERSPROC)GetProcAddress(Platform.Gdi32, "SwapBuffers");
}

/********************
 * Public Interface *
 ********************/

void
PlatformInit()
{
  if (Platform.Initialized)
  {
    return;
  }

  LoadWin32Libs();

  Platform.Instance = GetModuleHandleW(NULL);

  WNDCLASSEXA WindowClass = {};
  WindowClass.cbSize = sizeof(WNDCLASSEXA);
  WindowClass.lpszClassName = WIN32_DEFAULT_WNDCLASS_NAME;
  WindowClass.hInstance = Platform.Instance;
  WindowClass.hIcon = LoadIconW(NULL, IDI_WINLOGO);
  WindowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
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

void
PlatformShutdown()
{
  if (!Platform.Initialized)
  {
    return;
  }
  Platform.Initialized = false;
  UnregisterClassA(WIN32_DEFAULT_WNDCLASS_NAME, Platform.Instance);
}

bool
PlatformIsInitialized()
{
  return Platform.Initialized;
}

#endif