#pragma once

#include "Platform.h"

#ifdef SK_WINDOWS

#include <Windows.h>

typedef BOOL(WINAPI *DESTROYWINDOWPROC)(HWND);
constexpr DESTROYWINDOWPROC Win32DestroyWindow = DestroyWindow;
#undef DestroyWindow

typedef BOOL(WINAPI *SETWINDOWPOSPROC)(HWND, HWND, int, int, int, int, UINT);
constexpr SETWINDOWPOSPROC Win32SetWindowPos = SetWindowPos;
#undef SetWindowPos

// NOTE: Use CreateWidowExA
#undef CreateWindow
// NOTE: Use CreateFileA
#undef CreateFile

#include "Types.h"
#include "Event.h"
#include "Window.h"

typedef const char *(WINAPI *WGLGETEXTENSIONSSTRINGEXTPROC)(void);
extern WGLGETEXTENSIONSSTRINGEXTPROC Stak_wglGetExtensionsStringEXT;
#define wglGetExtensionsStringEXT Stak_wglGetExtensionsStringEXT
typedef const char *(WINAPI *WGLGETEXTENSIONSSTRINGARBPROC)(HDC);
extern WGLGETEXTENSIONSSTRINGARBPROC Stak_wglGetExtensionsStringARB;
#define wglGetExtensionsStringARB Stak_wglGetExtensionsStringARB
typedef HGLRC(WINAPI *WGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int *);
extern WGLCREATECONTEXTATTRIBSARBPROC Stak_wglCreateContextAttribsARB;
#define wglCreateContextAttribsARB Stak_wglCreateContextAttribsARB
typedef BOOL(WINAPI *WGLSWAPINTERVALEXTPROC)(int);
extern WGLSWAPINTERVALEXTPROC Stak_wglSwapIntervalEXT;
#define wglSwapIntervalEXT Stak_wglSwapIntervalEXT
typedef BOOL(WINAPI *WGLGETPIXELFORMATATTRIBIVARBPROC)(HDC, int, int, UINT, const int *, int *);
extern WGLGETPIXELFORMATATTRIBIVARBPROC Stak_wglGetPixelFormatAttribivARB;
#define wglGetPixelFormatAttribivARB Stak_wglGetPixelFormatAttribivARB
typedef BOOL(WINAPI *WGLCHOOSEPIXELFORMATARB)(HDC, const int *, const FLOAT *, UINT, int *, UINT *);
extern WGLCHOOSEPIXELFORMATARB Stak_wglChoosePixelFormatARB;
#define wglChoosePixelFormatARB Stak_wglChoosePixelFormatARB

constexpr const char *WIN32_DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";

LRESULT CALLBACK Win32MessageCallback(HWND, UINT, WPARAM, LPARAM);

struct platform
{
  HINSTANCE Instance;
  ATOM DefaultWindowClass;
  HWND DummyWindow;
  bool Initialized;
  HINSTANCE RendererApi;
  HINSTANCE User32;
  HINSTANCE Gdi32;
};

struct window
{
  HWND Handle;
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
  const char *Title;
  EventFn EventFunc;
  context *Context;
  void *Parent;
};

extern platform Platform;

#endif
